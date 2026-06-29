#pragma once
#include "Core/NoodleCore.h"
#include "Actor.h"

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
	std::vector<Camera*> m_Cameras;
	void AddActor(Actor* actor);
};