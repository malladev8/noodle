#include "NoodlePch.h"
#include "NoodleWindow.h"
#include "EngineContext.h"
#include "Event/EventManager.h"
#include "Event/WindowResizeEvent.h"

bool Window::Initialize(const WindowDesc& desc, EngineContext& engineContext)
{
	m_Width = desc.width;
	m_Height = desc.height;
	m_Title = desc.title;
	m_WindowResizeEventHandle = engineContext.eventManager.Subscribe<WindowResizeEvent>(
		[this](const WindowResizeEvent& event)
		{
			OnWindowResize(event);
		});

	return true;
}

void Window::Shutdown(EngineContext& engineContext)
{
	engineContext.eventManager.Unsubscribe(m_WindowResizeEventHandle);
}

float32 Window::GetAspectRatio() const
{
	return static_cast<float32>(m_Width) / static_cast<float32>(m_Height);
}

void Window::OnWindowResize(const WindowResizeEvent& event)
{
	m_Width = event.GetWidth();
	m_Height = event.GetHeight();
	N_LOG("Received Window Resize Event w:%i h:%i", event.GetWidth(), event.GetHeight());
}
