#include "NoodlePch.h"
#include "Scene.h"
#include "Actor/Actor.h"

void Scene::Update(int32 deltaMs)
{
	for (auto& actor : m_Actors)
	{
		actor->Update(deltaMs);
	}
}
