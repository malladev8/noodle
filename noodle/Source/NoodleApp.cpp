#include "NoodlePch.h"
#include "NoodleApp.h"
#include "Actor/SceneFactory.h"
#include "Event/EventManager.h"
#include "Event/WindowResizeEvent.h"

static const size_t APP_SIZE = 1_GB;

NoodleApp::NoodleApp()
{
}

NoodleApp::~NoodleApp()
{
	EventManager::GetGlobalEventManager()->Unsubscribe(m_WindowResizeEventHandle);
}

void NoodleApp::Init()
{
	// Testing scene creation
	SceneFactory sceneFactory;
	Scene* scene = sceneFactory.CreateScene("B:/ScarlettDev/RA/RedAngel/Cooked/Scene/TestScene.bin");
	m_Scene = std::unique_ptr<Scene>(scene);

	// Testing event manager
	m_WindowResizeEventHandle = EventManager::GetGlobalEventManager()->Subscribe<WindowResizeEvent>(
		[this](const WindowResizeEvent& event)
		{
			OnWindowResize(event);
		});
}

void NoodleApp::Run(float deltaSeconds)
{
	m_Scene->Update(deltaSeconds);
}

void NoodleApp::OnWindowResize(const WindowResizeEvent& event)
{
	N_LOG("Received Window Resize Event w:%i h:%i", event.GetWidth(), event.GetHeight());
}
