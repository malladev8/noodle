#include "NoodlePch.h"
#include "RedAngelApp.h"
#include "Actor/SceneFactory.h"
#include "Event/EventManager.h"

RedAngelApp::RedAngelApp(EngineContext& engineContext) : NoodleApp(engineContext)
{
}

void RedAngelApp::Initialize()
{
	NoodleApp::Initialize();

	// Testing scene creation
	SceneFactory sceneFactory;
	Scene* scene = sceneFactory.CreateScene("RedAngel/Cooked/Scene/TestScene.bin");
	m_Scene = std::unique_ptr<Scene>(scene);
}

void RedAngelApp::Update(float deltaSeconds)
{
	NoodleApp::Update(deltaSeconds);
}

void RedAngelApp::Shutdown()
{
	NoodleApp::Shutdown();
}
