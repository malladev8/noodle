#include "NoodlePch.h"
#include "D3D12Renderer.h"
#include "NoodleEngine.h"
#include "Event/WindowResizeEvent.h"

#if defined(WINDOWS)
D3D12Renderer::D3D12Renderer()
	: m_Viewport({}),
	  m_ScissorRect({}),
	  m_Device(nullptr),
	  m_SwapChain(nullptr),
	  m_CommandQueue(nullptr),
	  m_CommandList(nullptr),
	  m_Fence(nullptr),
	  m_FenceEvent(nullptr),
	  m_Factory(nullptr),
	  m_Adapter(nullptr),
	  m_FenceValue({}),
	  m_CurrentFenceValue(0),
	  m_RtvHandles({}),
	  m_RtvDescriptorSize(0),
	  m_CurrentFrameIndex(0),
	  m_WindowResizeEventHandle(0)
{
}

void D3D12Renderer::Initialize(void* hwnd, NoodleWindowDesc& windowDesc)
{
	// Initialize DXGI Factory
	UINT dxgiFactoryFlags = 0;

#if defined(DEBUG)
	EnableDebugLayer();
	dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif
	
	HRESULT hr = CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_Factory));
	if (FAILED(hr))
	{
		N_LOG("Failed to create DXGI Factory. HR: %i", hr);
		return;
	}
	 
	// Loop through adapters until DXGI_ERROR_NOT_FOUND is returned
	for (UINT adapterIndex = 0; SUCCEEDED(m_Factory->EnumAdapterByGpuPreference(adapterIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&m_Adapter))); ++adapterIndex)
	{
		DXGI_ADAPTER_DESC1 adapterDesc;
		m_Adapter->GetDesc1(&adapterDesc);

		// Skip software adapters
		if (adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			continue;
		}

		// Check D3D12 support
		if (SUCCEEDED(D3D12CreateDevice(m_Adapter.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)))
		{
			std::string adapterName = str::WideToUtf8(adapterDesc.Description);
			N_LOG("Using GPU adapter: %s", adapterName.c_str());
			break;
		}
	}

	// Create Device
	hr = D3D12CreateDevice(m_Adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_Device));
	if (FAILED(hr))
	{
		N_LOG("Failed to create D3D12 Device. HR: %i", hr);
		return;
	}

	// Create Command Queue
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT; // DIRECT supports graphics, copy, and compute.
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	hr = m_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_CommandQueue));
	if (FAILED(hr))
	{
		N_LOG("Failed to create D3D12 Command Queue. HR: %i", hr);
		return;
	}

	// Create Swap Chain
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = BufferCount;
	swapChainDesc.Width = windowDesc.width;
	swapChainDesc.Height = windowDesc.height;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;
	ComPtr<IDXGISwapChain1> swapChain1;
	hr = m_Factory->CreateSwapChainForHwnd(m_CommandQueue.Get(), static_cast<HWND>(hwnd), &swapChainDesc, nullptr, nullptr, &swapChain1);
	if (FAILED(hr))
	{
		N_LOG("Failed to create D3D12 Swap Chain. HR: %i", hr);
		return;
	}
	swapChain1.As(&m_SwapChain);
	m_CurrentFrameIndex = m_SwapChain->GetCurrentBackBufferIndex();

	// Create RTV (render target view) Descriptor Heap
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = BufferCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	hr = m_Device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_RtvHeap));
	if (FAILED(hr))
	{
		N_LOG("Failed to create D3D12 Descriptor Heap. HR: %i", hr);
		return;
	}
	m_RtvDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(rtvHeapDesc.Type);

	// Create RTVs (render target views)
	CreateRenderTargetViews();

	// Create Command Allocators
	for (uint32 i = 0; i < BufferCount; ++i)
	{
		hr = m_Device->CreateCommandAllocator(queueDesc.Type, IID_PPV_ARGS(&m_CommandAllocators[i]));
		if (FAILED(hr))
		{
			N_LOG("Failed to create D3D12 Command Allocator_%i. HR: %i", i, hr);
			return;
		}
	}

	// Create Command List
	uint32 nodeMask = 0;
	ID3D12PipelineState* initialPipelineState = nullptr;
	hr = m_Device->CreateCommandList(nodeMask, queueDesc.Type, m_CommandAllocators[0].Get(), initialPipelineState, IID_PPV_ARGS(&m_CommandList));
	if (FAILED(hr))
	{
		N_LOG("Failed to create D3D12 Command List. HR: %i", hr);
		return;
	}
	// D3D12 creates the command list in an open state. Close immediately.
	m_CommandList->Close();

	// Create Fence
	hr = m_Device->CreateFence(m_CurrentFenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence));
	if (FAILED(hr))
	{
		N_LOG("Failed to create D3D12 Fence. HR: %i", hr);
		return;
	}
	m_FenceEvent = CreateEvent(nullptr, false, false, nullptr);

	// Create Viewport and Scissor
	UpdateViewport(windowDesc.width, windowDesc.height);

	m_CurrentFrameIndex = m_SwapChain->GetCurrentBackBufferIndex();

	// Subsribe to WindowResizeEvent
	m_WindowResizeEventHandle = Engine::Get().GetContext().eventManager.Subscribe<WindowResizeEvent>(
		[this](const WindowResizeEvent& event)
		{
			Resize(event);
		});
}

void D3D12Renderer::Shutdown()
{
	// Unsubscribe from WindowResizeEvent
	Engine::Get().GetContext().eventManager.Unsubscribe(m_WindowResizeEventHandle);

	// Shutdown
}

void D3D12Renderer::BeginFrame()
{
	// Prepare the command list for recording
	// Wait for GPU to finish this frame’s resources
	WaitForGpu();

	// Reset allocator for this frame
	HRESULT hr = m_CommandAllocators[m_CurrentFrameIndex]->Reset();
	N_ASSERT(!FAILED(hr), "Failed Command Allocator Reset.");

	// Reset command list
	hr = m_CommandList->Reset(m_CommandAllocators[m_CurrentFrameIndex].Get(), nullptr); // no PSO yet
	N_ASSERT(!FAILED(hr), "Failed Command List Reset.");

	// Transition backbuffer: PRESENT → RENDER_TARGET
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargets[m_CurrentFrameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	m_CommandList->ResourceBarrier(1, &barrier);

	// Get RTV handle for this frame
	D3D12_CPU_DESCRIPTOR_HANDLE rtv = m_RtvHandles[m_CurrentFrameIndex];
	m_CommandList->OMSetRenderTargets(1, &rtv, false, nullptr);

	// Clear
	const float clearColor[] = { 1.0f, 0.0f, 0.0f, 1.0f };
	m_CommandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
}

void D3D12Renderer::RenderFrame()
{
	// TODO:
	// DrawSprite()
	// DrawWhatever()...
}

void D3D12Renderer::EndFrame()
{
	// Finalize the command list and transition back
	// Transition: RENDER_TARGET → PRESENT
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargets[m_CurrentFrameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	m_CommandList->ResourceBarrier(1, &barrier);
	HRESULT hr = m_CommandList->Close();
	N_ASSERT(!FAILED(hr), "Failed Command List Close.");
}

void D3D12Renderer::Present()
{
	// Execute command list and present
	ID3D12CommandList* commandLists[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);
	HRESULT hr = m_SwapChain->Present(1, 0);
	N_ASSERT(!FAILED(hr), "Failed Present.");

	// Signal fence
	++m_CurrentFenceValue;
	hr = m_CommandQueue->Signal(m_Fence.Get(), m_CurrentFenceValue);
	m_FenceValue[m_CurrentFrameIndex] = m_CurrentFenceValue;
	N_ASSERT(!FAILED(hr), "Failed Fence Signal.");

	m_CurrentFrameIndex = m_SwapChain->GetCurrentBackBufferIndex();
}

void D3D12Renderer::SubmitSprite(const SpriteRenderCommand& cmd)
{
}

void D3D12Renderer::Resize(const WindowResizeEvent& event)
{
	uint32 width = event.GetWidth();
	uint32 height = event.GetHeight();

	if (width == 0 || height == 0)
	{
		return;
	}

	WaitForGpu();

	for (uint32 i = 0; i < BufferCount; ++i)
	{
		m_RenderTargets[i].Reset();
	}

	HRESULT hr = m_SwapChain->ResizeBuffers(BufferCount, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
	N_ASSERT(!FAILED(hr), "Failed to resize swap chain buffers.");
	m_CurrentFrameIndex = m_SwapChain->GetCurrentBackBufferIndex();
	CreateRenderTargetViews();
	UpdateViewport(width, height);
}

void D3D12Renderer::EnableDebugLayer()
{
	ComPtr<ID3D12Debug> debugController = nullptr;
	D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
	debugController->EnableDebugLayer();
}

void D3D12Renderer::WaitForGpu()
{
	if (m_Fence->GetCompletedValue() < m_FenceValue[m_CurrentFrameIndex])
	{
		m_Fence->SetEventOnCompletion(m_FenceValue[m_CurrentFrameIndex], m_FenceEvent);
		WaitForSingleObject(m_FenceEvent, INFINITE);
	}
}

void D3D12Renderer::CreateRenderTargetViews()
{
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_RtvHeap->GetCPUDescriptorHandleForHeapStart();

	// Create RTVs (render target views)
	for (uint32 i = 0; i < BufferCount; ++i)
	{
		// Get back buffer resources from swap chain and store as render targets
		HRESULT hr = m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_RenderTargets[i]));
		if (FAILED(hr))
		{
			N_LOG("Failed to get D3D12 Buffer_%i from swap chain. HR: %i", i, hr);
			return;
		}

		// Create RTV
		m_Device->CreateRenderTargetView(m_RenderTargets[i].Get(), nullptr /*D3D12_RENDER_TARGET_VIEW_DESC*/, rtvHandle);
		m_RtvHandles[i] = rtvHandle;
		rtvHandle.ptr += m_RtvDescriptorSize;
	}
}
void D3D12Renderer::UpdateViewport(uint32 width, uint32 height)
{
	m_Viewport.TopLeftX = 0.0f;
	m_Viewport.TopLeftY = 0.0f;
	m_Viewport.Width = (float32)width;
	m_Viewport.Height = (float32)height;
	m_Viewport.MinDepth = 0.0f;
	m_Viewport.MaxDepth = 1.0f;

	m_ScissorRect = { 0, 0, (long)width, (long)height };
}
#endif