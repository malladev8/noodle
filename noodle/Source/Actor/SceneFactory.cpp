#include "NoodlePch.h"
#include "SceneFactory.h"
#include "Scene.h"
#include "ActorFactory.h"
#include "Resource/ResourceTypes.h"

#include <fstream>

SceneFactory::SceneFactory()
{
}

Scene* SceneFactory::CreateScene(const char* binPath)
{
	std::ifstream bin(binPath, std::ios::binary);
	if (!bin.is_open())
	{
		N_LOG("Failed to open Scene binary file %s", binPath);
		return N_NEW Scene();
	}

	Scene* scene = N_NEW Scene();

	// Scene Asset Id
	AssetId sceneId = 0;
	bin.read(reinterpret_cast<char*>(&sceneId), sizeof(AssetId));

	// Create Actors
	uint32 numActors = 0;
	bin.read(reinterpret_cast<char*>(&numActors), sizeof(uint32));
	ActorFactory actorFactory;
	for (uint32 i = 0; i < numActors; ++i)
	{
		std::string actorPrefabPath = path::ReadFilePath(bin);
		Actor* actor = actorFactory.CreateActor(actorPrefabPath.data(), &bin);
		scene->AddActor(actor);
	}

	// Set up parent actor relationships
	for (uint32 i = 0; i < numActors; ++i)
	{
		Actor& actor = *scene->m_Actors[i].get();
		if (actor.GetParentId().has_value())
		{
			actor.SetParent(scene->m_ActorLookup[actor.GetParentId().value()]);
		}
	}

	bin.close();
	return scene;
}
