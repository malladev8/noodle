#include "NoodlePch.h"
#include "Scene.h"

void Scene::Update(float deltaSeconds)
{
	for (auto& actor : m_Actors)
	{
		actor->Update(deltaSeconds);
	}
}

void Scene::AddActor(Actor* actor)
{
	N_ASSERT(actor != nullptr, "Cannot add null actor to scene.");
	m_Actors.push_back(std::unique_ptr<Actor>(actor));
}
