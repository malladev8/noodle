#pragma once
#include "Core/NoodleCore.h"
#include "Actor.h"

class Actor;
class IRenderer;

class Scene
{
	friend class SceneFactory;
public:
	void Update(float deltaSeconds);
	void Render(IRenderer& IRenderer);
protected:
private:
	std::vector<std::unique_ptr<Actor>> m_Actors;
	void AddActor(Actor* actor);
};