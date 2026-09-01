#include "NoodlePch.h"
#include "NoodleEngine.h"
#include "NoodleWindow.h"
#include "Rendering/IRenderer.h"
#include "Actor/SceneFactory.h"

#if defined(WINDOWS)
#include "Rendering/D3D12/D3D12Renderer.h"
#endif

Engine::Engine()
	: m_EventManager(EventManager("GlobalEventManager")),
	  m_Renderer(nullptr),
	  m_ResourceManager(ResourceManager()),
	  m_InputManager(InputManager()),
	  m_EngineContext(
		  {
			  m_EventManager, 
			  m_Renderer.get(),
			  m_ResourceManager,
			  m_InputManager,
			  m_Window,
			  m_Clock
		  }
	  )
{
}

Engine& Engine::Get()
{
	static Engine sEngineInstance;
	return sEngineInstance;
}

void Engine::Initialize()
{
	// Initialization
	m_Clock.Reset();

	PlatformInitLogger();

	WindowDesc windowDesc;
	windowDesc.width = 1280;
	windowDesc.height = 720;
	windowDesc.title = "MainWindow";
	m_Window.Initialize(windowDesc, m_EngineContext);

	void* hwnd = nullptr;
	PlatformCreateWindow(m_Window, hwnd);

#if defined(WINDOWS)
	m_Renderer = std::make_unique<D3D12Renderer>();
#endif
	if (m_Renderer)
	{
		m_Renderer->Initialize(hwnd, m_Window);
	}
	m_EngineContext.renderer = m_Renderer.get();
}

void Engine::BeginFrame()
{
	m_InputManager.BeginFrame();
}

void Engine::Update(float32 deltaSeconds)
{
	PlatformDispatchMessages();
	m_EventManager.Update(deltaSeconds);
	m_ActiveScene->Update(deltaSeconds);
}

void Engine::Render()
{
	m_ActiveScene->SubmitRenderCommands(*m_Renderer.get());
	m_ActiveScene->Render(*m_Renderer.get(), m_Window);
}

void Engine::EndFrame()
{
}

void Engine::Shutdown()
{
	m_Renderer->Shutdown();
	m_Window.Shutdown(m_EngineContext);
	PlatformShutdown();
}

Scene* Engine::LoadScene(const char* scenePath)
{
	SceneFactory sceneFactory;
	Scene* scene = sceneFactory.CreateScene(scenePath);
	m_ActiveScene = std::unique_ptr<Scene>(scene);
	return m_ActiveScene.get();
}

float32 Engine::TickClock()
{
	m_Clock.Tick();
	return m_Clock.GetDeltaSeconds();
}