#include "NoodlePch.h"
#include "NoodleApp.h"
#include "Actor/SceneFactory.h"

static const size_t APP_SIZE = 1_GB;

NoodleApp::NoodleApp()
{
}

NoodleApp::~NoodleApp()
{
}

void NoodleApp::Init()
{
	// Testing scene creation
	SceneFactory sceneFactory;
	Scene* scene = sceneFactory.CreateScene("B:/ScarlettDev/RA/RedAngel/Cooked/Scene/TestScene.bin");
	m_Scene = std::unique_ptr<Scene>(scene);
}

void NoodleApp::Run(float deltaSeconds)
{
	m_Scene->Update(deltaSeconds);
}
