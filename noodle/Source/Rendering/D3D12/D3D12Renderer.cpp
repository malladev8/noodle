#include "NoodlePch.h"
#include "D3D12Renderer.h"
#include "NoodleEngine.h"
#include "NoodleWindow.h"
#include "Event/WindowResizeEvent.h"
#include "D3D12SpriteRenderer.h"

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
	  m_RtvHeap(nullptr),
	  m_SrvHeap(nullptr),
	  m_FrameConstantBuffer(nullptr),
	  m_MappedFrameCB(nullptr),
	  m_FrameCBGpuAddress(0),
	  m_UploadAllocator(nullptr),
	  m_UploadCommandList(nullptr),
	  m_UploadFence(nullptr),
	  m_UploadFenceEvent(nullptr),
	  m_SpriteRenderer(nullptr),
	  m_RenderTargets({}),
	  m_CommandAllocators({}),
	  m_FenceValue({}),
	  m_CurrentFenceValue(0),
	  m_UploadFenceValue(0),
	  m_RtvHandles({}),
	  m_RtvDescriptorSize(0),
	  m_CurrentFrameIndex(0),
	  m_WindowResizeEventHandle(0),
	  m_SrvDescriptorSize(0),
	  m_CurrentSrvIndex(0)
{
}

void D3D12Renderer::Initialize(void* hwnd, Window& windowDesc)
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
	swapChainDesc.Width = windowDesc.GetWidth();
	swapChainDesc.Height = windowDesc.GetHeight();
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

	hr = m_Device->CreateCommandAllocator(queueDesc.Type, IID_PPV_ARGS(&m_UploadAllocator));
	if (FAILED(hr))
	{
		N_LOG("Failed to create D3D12 Upload Command Allocator. HR: %i",  hr);
		return;
	}

	// Create Command Lists
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

	hr = m_Device->CreateCommandList(nodeMask, queueDesc.Type, m_UploadAllocator.Get(), initialPipelineState, IID_PPV_ARGS(&m_UploadCommandList));
	if (FAILED(hr))
	{
		N_LOG("Failed to create D3D12 Upload Command List. HR: %i", hr);
		return;
	}
	m_UploadCommandList->Close();

	// Create Fences
	hr = m_Device->CreateFence(m_CurrentFenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence));
	if (FAILED(hr))
	{
		N_LOG("Failed to create D3D12 Fence. HR: %i", hr);
		return;
	}
	m_FenceEvent = CreateEvent(nullptr, false, false, nullptr);

	hr = m_Device->CreateFence(m_UploadFenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_UploadFence));
	if (FAILED(hr))
	{
		N_LOG("Failed to create D3D12 Upload Fence. HR: %i", hr);
		return;
	}
	m_UploadFenceEvent = CreateEvent(nullptr, false, false, nullptr);

	// Create Viewport and Scissor
	UpdateViewport(windowDesc.GetWidth(), windowDesc.GetHeight());

	m_CurrentFrameIndex = m_SwapChain->GetCurrentBackBufferIndex();

	// Create SRV (shader resource view) heap
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = 1024;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	hr = m_Device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_SrvHeap));
	if (FAILED(hr))
	{
		N_LOG("Failed to create D3D12 SRV Descriptor Heap. HR: %i", hr);
		return;
	}
	m_SrvDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// Initialize Sub Renderers
	m_SpriteRenderer = std::make_unique<D3D12SpriteRenderer>();
	m_SpriteRenderer->Initialize(*this);

	// Initialize Frame Constant Buffer
	uint32 cbSize = align::Align256(sizeof(FrameConstants)); // D3D12 requires all constant buffers to be 256 byte-aligned

	D3D12_HEAP_PROPERTIES heapProps = {};
	heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Width = cbSize;
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	hr = m_Device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_FrameConstantBuffer));
	if (FAILED(hr))
	{
		N_LOG("Failed to create committed resource for Frame Constant Buffer. HR: %i", hr);
		return;
	}

	D3D12_RANGE readRange = { 0, 0 };
	// Common in D3D12 to leave upload CBs mapped forever like this
	m_FrameConstantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&m_MappedFrameCB));
	
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

void D3D12Renderer::BeginFrame(const CameraData& camData)
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

	// Update Frame Constant Buffer
	FrameConstants frameConstants;
	frameConstants.view = camData.view.Transpose();
	frameConstants.projection = camData.projection.Transpose();
	frameConstants.viewProjection = camData.projection * camData.view;
	frameConstants.viewProjection = frameConstants.viewProjection.Transpose();
	memcpy(m_MappedFrameCB, &frameConstants, sizeof(frameConstants));

	// Clear
	const float clearColor[] = { camData.clearColor.r, camData.clearColor.g, camData.clearColor.b, camData.clearColor.a };
	m_CommandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
}

void D3D12Renderer::RenderFrame()
{
	D3D12_VIEWPORT viewports[]{ m_Viewport };
	m_CommandList->RSSetViewports(1, viewports);

	D3D12_RECT scissorRects[]{ m_ScissorRect };
	m_CommandList->RSSetScissorRects(1, scissorRects);

	// Flush Sub-renderers - convert queued render commands into GPU draw calls
	m_SpriteRenderer->Flush();
	// TODO: m_DebugRenderer->Flush();
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
	N_ASSERT(!FAILED(hr), "Failed Fence Signal.");
	m_FenceValue[m_CurrentFrameIndex] = m_CurrentFenceValue;
	m_CurrentFrameIndex = m_SwapChain->GetCurrentBackBufferIndex();
}

void D3D12Renderer::SubmitSprite(const SpriteRenderCommand& cmd)
{
	m_SpriteRenderer->SubmitSprite(cmd);
}

void D3D12Renderer::CreateTextureResources(Texture& texture, const std::vector<uint8>& pixels)
{
	std::unique_ptr<D3D12TextureResource> d3d12Texture = std::make_unique<D3D12TextureResource>();

	// Create default heap texture
	D3D12_RESOURCE_DESC textureDesc = {};
	textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureDesc.Width = texture.width;
	textureDesc.Height = texture.height;
	textureDesc.DepthOrArraySize = 1;
	textureDesc.MipLevels = 1;
	textureDesc.Format = GetDxgiFormat(texture.format);
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	CD3DX12_HEAP_PROPERTIES defaultHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	// Using D3D12_RESOURCE_STATE_COPY_DEST because we're uploading onto it.
	HRESULT hr = m_Device->CreateCommittedResource(&defaultHeap, D3D12_HEAP_FLAG_NONE, &textureDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&d3d12Texture->textureResource));
	N_ASSERT(!FAILED(hr), "Failed to create Texture resource.");

	// Create upload heap
	uint64 uploadBufferSize = 0;
	m_Device->GetCopyableFootprints(&textureDesc, 0, 1, 0, nullptr, nullptr, nullptr, &uploadBufferSize);

	CD3DX12_HEAP_PROPERTIES uploadHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC uploadDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
	hr = m_Device->CreateCommittedResource(&uploadHeap, D3D12_HEAP_FLAG_NONE, &uploadDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&d3d12Texture->uploadResource));
	N_ASSERT(!FAILED(hr), "Failed to create upload resource.");

	// Prepare subresource data
	D3D12_SUBRESOURCE_DATA subresourceData = {};
	subresourceData.pData = pixels.data();
	subresourceData.RowPitch = texture.width * 4; // TODO: Implement helper to return bytes per pixel. For now, RGBA8 = 4 bytes per pixel
	subresourceData.SlicePitch = subresourceData.RowPitch * texture.height;

	// Upload texture data
	hr = m_UploadAllocator->Reset();
	N_ASSERT(!FAILED(hr), "Failed to reset upload allocator.");

	hr = m_UploadCommandList->Reset(m_UploadAllocator.Get(), nullptr);
	N_ASSERT(!FAILED(hr), "Failed to reset upload command list during texture resource creation.");

	// Helper from d3dx12.h
	// Internally: maps upload heap, copies CPU data, records copy commands
	UpdateSubresources(m_UploadCommandList.Get(), d3d12Texture->textureResource.Get(), d3d12Texture->uploadResource.Get(), 0, 0, 1, &subresourceData);

	// Transition texture resource state COPY_DEST -> PIXEL_SHADER_RESOURCE
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(d3d12Texture->textureResource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	m_UploadCommandList->ResourceBarrier(1, &barrier);
	hr = m_UploadCommandList->Close();
	N_ASSERT(!FAILED(hr), "Failed to close upload command list during texture resource creation.");

	ID3D12CommandList* uploadLists[] = { m_UploadCommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(1, uploadLists);
	hr = m_CommandQueue->Signal(m_UploadFence.Get(), ++m_UploadFenceValue);
	N_ASSERT(!FAILED(hr), "Failed to signal upload fence.");
	WaitForGpuUpload();

	// Clean up upload resource
	d3d12Texture->uploadResource.Reset();

	// Create SRV
	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_SrvHeap->GetCPUDescriptorHandleForHeapStart());
	cpuHandle.Offset(m_CurrentSrvIndex, m_SrvDescriptorSize);

	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(m_SrvHeap->GetGPUDescriptorHandleForHeapStart());
	gpuHandle.Offset(m_CurrentSrvIndex, m_SrvDescriptorSize);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = GetDxgiFormat(texture.format);
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	m_Device->CreateShaderResourceView(d3d12Texture->textureResource.Get(), &srvDesc, cpuHandle);
	d3d12Texture->srvCpuHandle = cpuHandle;
	d3d12Texture->srvGpuHandle = gpuHandle;

	// Advance heap allocation
	++m_CurrentSrvIndex;

	// Store D3D12 backend resource
	texture.resource = std::move(d3d12Texture);
}

void D3D12Renderer::CreateShaderResources(std::shared_ptr<struct Shader>& outShader, const CookedShaderData& shaderData)
{
	// TODO: Need logic for which sub-renderer to use.
	m_SpriteRenderer->CreateShaderResources(outShader, shaderData);
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

void D3D12Renderer::WaitForGpuUpload()
{
	if (m_UploadFence->GetCompletedValue() < m_UploadFenceValue)
	{
		m_UploadFence->SetEventOnCompletion(m_UploadFenceValue, m_UploadFenceEvent);
		WaitForSingleObject(m_UploadFenceEvent, INFINITE);
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

DXGI_FORMAT D3D12Renderer::GetDxgiFormat(eTextureFormat format)
{
	switch (format)
	{
	case eTextureFormat::R8_UNORM:
		return DXGI_FORMAT_R8_UNORM;
	case eTextureFormat::RG8_UNORM:
		return DXGI_FORMAT_R8G8_UNORM;
	case eTextureFormat::RGBA8_UNORM:
		return DXGI_FORMAT_R8G8B8A8_UNORM;
	case eTextureFormat::RGBA8_UNORM_SRGB:
		return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	case eTextureFormat::BGRA8_UNORM:
		return DXGI_FORMAT_B8G8R8A8_UNORM;
	case eTextureFormat::D24_UNORM_S8_UINT:
		return DXGI_FORMAT_D24_UNORM_S8_UINT;
	case eTextureFormat::D32_FLOAT:
		return DXGI_FORMAT_D32_FLOAT;
	default:
		return DXGI_FORMAT_UNKNOWN;
	}
}
#endif