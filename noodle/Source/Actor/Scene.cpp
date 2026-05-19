#include "../NoodlePch.h"
#include "Scene.h"

void Scene::Update(int32 deltaMs)
{
	for (auto& actor : m_Actors)
	{
		actor->Update(deltaMs);
	}
}

void Scene::AddActor(Actor* actor)
{
	N_ASSERT(actor != nullptr, "Cannot add null actor to scene.");
	m_Actors.push_back(std::unique_ptr<Actor>(actor));
}
