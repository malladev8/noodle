#pragma once
#include "Core/NoodleCore.h"
#include "Scene.h"

class SceneFactory
{
public:
	SceneFactory();
	Scene* CreateScene(const char* binPath);

protected:
private:
};
