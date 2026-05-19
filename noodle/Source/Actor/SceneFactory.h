#pragma once
#include "../Core/NoodleCore.h"

class Scene;

class SceneFactory
{
public:
	SceneFactory();
	Scene* CreateScene(const char* binPath);

protected:
private:
};
