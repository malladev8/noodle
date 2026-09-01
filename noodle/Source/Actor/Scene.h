#pragma once
#include "Core/NoodleCore.h"
#include "Actor.h"
#include <unordered_map>
#include "Controllers/Controller.h"

class Actor;
class IRenderer;
class Camera;
class Window;
class Controller;

class Scene
{
	friend class SceneFactory;
public:
	void Update(float deltaSeconds);
	void SubmitRenderCommands(IRenderer& IRenderer) const;
	void Render(IRenderer& IRenderer, const Window& window) const;

	template<class ControllerType>
	std::vector<ControllerType*> CreateControllers(const std::string& controllerType)
	{
		std::vector<ControllerType*> controllers;
		for (std::unique_ptr<Actor>& actor : m_Actors)
		{
			if (actor.get()->GetControllerType() == controllerType)
			{
				ControllerType* controller = N_NEW ControllerType();
				controller->Possess(actor.get());
				controllers.push_back(controller);
				m_Controllers.push_back(std::unique_ptr<Controller>(controller));
			}
		}
		return controllers;
	}

protected:
private:
	std::vector<std::unique_ptr<Actor>> m_Actors;
	std::unordered_map<ActorId, Actor*> m_ActorLookup;
	std::vector<Camera*> m_Cameras;
	std::vector<std::unique_ptr<Controller>> m_Controllers;
	void AddActor(Actor* actor);
};