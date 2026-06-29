#include "NoodlePch.h"
#include "Scene.h"
#include "Components/Camera.h"
#include "NoodleWindow.h"
#include "Rendering/IRenderer.h"


void Scene::Update(float deltaSeconds)
{
	for (auto& actor : m_Actors)
	{
		actor->Update(deltaSeconds);
	}
}

void Scene::SubmitRenderCommands(IRenderer& IRenderer) const
{
	// TODO: Separate Actors with renderable components into separate array
	// Currently we loop through every actor and all of their components again each frame
	// even if there's nothing to render.
	for (auto& actor : m_Actors)
	{
		actor->SubmitRenderCommands(IRenderer);
	}
}

void Scene::Render(IRenderer& IRenderer, const Window& window) const
{
	N_ASSERT(!m_Cameras.empty(), "Scene has no actors with camera components. Cannot render.");

	// TODO: Need to add support for multiple cameras. For now just use the first camera.
	IRenderer.BeginFrame(m_Cameras[0]->BuildCameraData(window.GetAspectRatio()));
	IRenderer.RenderFrame();
	IRenderer.EndFrame();
	IRenderer.Present();
}

void Scene::AddActor(Actor* actor)
{
	N_ASSERT(actor != nullptr, "Cannot add null actor to scene.");
	m_Actors.push_back(std::unique_ptr<Actor>(actor));

	Camera* camera = actor->GetComponent<Camera>();
	if (camera != nullptr)
	{
		m_Cameras.push_back(camera);
	}
}
