#include "NoodlePch.h"
#include "NoodleApp.h"
#include "Actor/SceneFactory.h"
#include "EngineContext.h"
#include "Event/EventManager.h"
#include "Event/WindowResizeEvent.h"
#include "Rendering/IRenderer.h"

static const size_t APP_SIZE = 1_GB;

NoodleApp::NoodleApp(EngineContext& engineContext)
	: m_EngineContext(engineContext)
{
}

NoodleApp::~NoodleApp()
{
}

void NoodleApp::Initialize()
{
	// Testing event manager
	m_WindowResizeEventHandle = m_EngineContext.eventManager.Subscribe<WindowResizeEvent>(
		[this](const WindowResizeEvent& event)
		{
			OnWindowResize(event);
		});
}

void NoodleApp::Update(float deltaSeconds)
{
	m_Scene->Update(deltaSeconds);
}

void NoodleApp::SubmitRenderCommands(IRenderer& IRenderer)
{
	m_Scene->SubmitRenderCommands(IRenderer);
}

void NoodleApp::Shutdown()
{
	m_EngineContext.eventManager.Unsubscribe(m_WindowResizeEventHandle);
}

void NoodleApp::OnWindowResize(const WindowResizeEvent& event)
{
	N_LOG("Received Window Resize Event w:%i h:%i", event.GetWidth(), event.GetHeight());
}
