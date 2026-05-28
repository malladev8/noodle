#include "NoodlePch.h"
#include "RedAngelApp.h"
#include "Actor/SceneFactory.h"
#include "Event/EventManager.h"

RedAngelApp::RedAngelApp(EngineContext& engineContext) : NoodleApp(engineContext)
{
}

void RedAngelApp::Init()
{
	NoodleApp::Init();

	// Testing scene creation
	SceneFactory sceneFactory;
	Scene* scene = sceneFactory.CreateScene("RedAngel/Cooked/Scene/TestScene.bin");
	m_Scene = std::unique_ptr<Scene>(scene);
}

void RedAngelApp::Run(float deltaSeconds)
{
	NoodleApp::Run(deltaSeconds);
	m_Scene->Update(deltaSeconds);
}

void RedAngelApp::Shutdown()
{
	NoodleApp::Shutdown();
}
