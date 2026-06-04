#include "NoodlePch.h"
#include "Scene.h"

void Scene::Update(float deltaSeconds)
{
	for (auto& actor : m_Actors)
	{
		actor->Update(deltaSeconds);
	}
}

void Scene::Render(IRenderer& IRenderer)
{
	// TODO: Separate Actors with renderable components into separate array
	// Currently we loop through every actor and all of their components again each frame
	// even if there's nothing to render.
	for (auto& actor : m_Actors)
	{
		actor->Render(IRenderer);
	}
}

void Scene::AddActor(Actor* actor)
{
	N_ASSERT(actor != nullptr, "Cannot add null actor to scene.");
	m_Actors.push_back(std::unique_ptr<Actor>(actor));
}
