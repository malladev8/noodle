#include "NoodlePch.h"
#include "D3D12Renderer.h"
#include "NoodleEngine.h"
#include "Event/WindowResizeEvent.h"

#if defined(WINDOWS)
D3D12Renderer::D3D12Renderer()
	: m_Device(nullptr),
	  m_SwapChain(nullptr),
	  m_CommandQueue(nullptr),
	  m_CommandList(nullptr),
	  m_Fence(nullptr),
	  m_FenceEvent(nullptr),
	  m_Factory(nullptr),
	  m_Adapter(nullptr),
	  m_FenceValue(0),
	  m_WindowResizeEventHandle(0)
{
}

void D3D12Renderer::Initialize(void* hwnd)
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
		DXGI_ADAPTER_DESC1 desc;
		m_Adapter->GetDesc1(&desc);

		// Skip software adapters
		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			continue;
		}

		// Check D3D12 support
		if (SUCCEEDED(D3D12CreateDevice(m_Adapter.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)))
		{
			std::string adapterName = str::WideToUtf8(desc.Description);
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
}

void D3D12Renderer::RenderFrame()
{
}

void D3D12Renderer::EndFrame()
{
}

void D3D12Renderer::Present()
{
}

void D3D12Renderer::SubmitSprite(const SpriteRenderCommand& cmd)
{
}

void D3D12Renderer::Resize(const WindowResizeEvent& event)
{
	// TODO: Handle resize
}

void D3D12Renderer::EnableDebugLayer()
{
	ComPtr<ID3D12Debug> debugController = nullptr;
	D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
	debugController->EnableDebugLayer();
}
#endif