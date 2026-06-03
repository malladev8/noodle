#include "NoodlePch.h"
#include "D3D12Renderer.h"
#include "NoodleEngine.h"
#include "Event/WindowResizeEvent.h"

#if defined(WINDOWS)
void D3D12Renderer::Initialize(void* hwnd)
{
	// Init First

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
#endif