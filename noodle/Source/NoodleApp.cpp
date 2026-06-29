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
}

void NoodleApp::Update(float deltaSeconds)
{
	m_Scene->Update(deltaSeconds);
}

void NoodleApp::Render(IRenderer& IRenderer)
{
	m_Scene->SubmitRenderCommands(IRenderer);
	m_Scene->Render(IRenderer, m_EngineContext.window);
}

void NoodleApp::Shutdown()
{
}