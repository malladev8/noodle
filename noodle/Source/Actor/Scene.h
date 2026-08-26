#pragma once
#include "Core/NoodleCore.h"
#include "Actor.h"
#include <unordered_map>
#include "Controllers/Controller.h"

class Actor;
class IRenderer;
class Camera;
class Window;

class Scene
{
	friend class SceneFactory;
public:
	void Update(float deltaSeconds);
	void SubmitRenderCommands(IRenderer& IRenderer) const;
	void Render(IRenderer& IRenderer, const Window& window) const;

protected:
private:
	std::vector<std::unique_ptr<Actor>> m_Actors;
	std::unordered_map<ActorId, Actor*> m_ActorLookup;
	std::vector<Camera*> m_Cameras;
	std::vector<std::unique_ptr<class Controller>> m_Controllers;
	void AddActor(Actor* actor);
};