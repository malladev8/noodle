#include "NoodlePch.h"
#include "RedAngelApp.h"
#include "Event/EventManager.h"
#include "EngineContext.h"
#include "Event/WindowResizeEvent.h"
#include "Rendering/IRenderer.h"
#include "NoodleEngine.h"
#include "Actor/Scene.h"
#include "Controllers/PlayerController.h"

RedAngelApp::RedAngelApp()
{
}

void RedAngelApp::Run()
{
	Initialize();

	while (!PlatformShouldExit())
	{
		float32 deltaSeconds = Engine::Get().TickClock();
		BeginFrame();
		Update(deltaSeconds);
		Render();
		EndFrame();
	}

	Shutdown();
}

void RedAngelApp::Initialize()
{
	Engine::Get().Initialize();

	m_ProjectRoot = std::filesystem::current_path();
	m_CookedAssetRoot = m_ProjectRoot / "Cooked";

	// Testing scene creation
	std::filesystem::path scenePath = m_CookedAssetRoot / "Scene" / "TestScene.bin";
	Scene* scene = Engine::Get().LoadScene(scenePath.string().c_str());

	// Create Controllers
	std::vector<PlayerController*> playerControllers = scene->CreateControllers<PlayerController>("Player");
}

void RedAngelApp::BeginFrame()
{
	Engine::Get().BeginFrame();
}

void RedAngelApp::Update(float deltaSeconds)
{
	Engine::Get().Update(deltaSeconds);
}

void RedAngelApp::Render()
{
	Engine::Get().Render();
}

void RedAngelApp::EndFrame()
{
	Engine::Get().EndFrame();
}

void RedAngelApp::Shutdown()
{
	Engine::Get().Shutdown();
}
