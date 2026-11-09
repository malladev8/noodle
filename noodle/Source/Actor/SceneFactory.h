#pragma once
#include "Core/NoodleCore.h"
#include "Scene.h"

class SceneFactory
{
public:
	SceneFactory();
	Scene* CreateScene(const char* binPath, const SceneInitParameters& sceneInitParamters);

protected:
private:
};
