#include "NoodlePch.h"
#include "Scene.h"
#include "Components/Camera.h"
#include "Components/Collider.h"
#include "Controllers/Controller.h"
#include "NoodleWindow.h"
#include "Rendering/IRenderer.h"

Scene::~Scene()
{
    Destroy();
}

void Scene::PostInit()
{
    // Resolve parent relationships only after all actors exist.
    for (const std::unique_ptr<Actor>& actor : m_Actors)
    {
        if (actor->GetParentId().has_value())
        {
            auto it = m_ActorLookup.find(actor->GetParentId().value());

            N_ASSERT(it != m_ActorLookup.end(), "Actor references a parent that doesn't exist.");

            if (it != m_ActorLookup.end())
            {
                actor->SetParent(it->second);
            }
        }
    }

    // Actor PostInit
    for (const std::unique_ptr<Actor>& actor : m_Actors)
    {
        actor->PostInit();
    }

    // Now that actors and their ownership relationships are established,
    // register colliders with this scene's physics system.
    for (const std::unique_ptr<Actor>& actor : m_Actors)
    {
        if (Collider* collider = actor->GetComponent<Collider>())
        {
            collider->RegisterWithPhysics(m_Physics);
        }
    }

    
}

void Scene::Destroy()
{
    if (m_Destroyed)
    {
        return;
    }

    m_Destroyed = true;

    // Controllers may hold pointers to actors, so destroy them first.
    for (std::unique_ptr<Controller>& controller : m_Controllers)
    {
        controller->UnPossess();
    }
    m_Controllers.clear();

    // Remove cached camera references
    m_Cameras.clear();

    // Explicitly destroy actors while their components still exist.
    for (const std::unique_ptr<Actor>& actor : m_Actors)
    {
        actor->Destroy();
    }

    m_ActorLookup.clear();
    m_Actors.clear();
}

void Scene::Update(float deltaSeconds)
{
	for (auto& controller : m_Controllers)
	{
		controller->Update(deltaSeconds);
	}

	for (auto& actor : m_Actors)
	{
		actor->Update(deltaSeconds);
	}

	m_Physics.Update(deltaSeconds);
}

void Scene::SubmitRenderCommands(IRenderer& IRenderer) const
{
	// TODO: Separate Actors with renderable components into separate array
	// Currently we loop through every actor and all of their components again each frame
	// even if there's nothing to render.
	for (auto& actor : m_Actors)
	{
		actor->SubmitRenderCommands(IRenderer);
	}

    m_Physics.SubmitDebugRenderCommands(IRenderer);
}

void Scene::Render(IRenderer& IRenderer, const Window& window) const
{
	N_ASSERT(!m_Cameras.empty(), "Scene has no actors with camera components. Cannot render.");

	// TODO: Need to add support for multiple cameras. For now just use the first camera.
	IRenderer.BeginFrame(m_Cameras[0]->BuildCameraData(window.GetAspectRatio()));
	IRenderer.RenderFrame();
	IRenderer.EndFrame();
	IRenderer.Present();
}

void Scene::AddActor(Actor* actor)
{
	N_ASSERT(actor != nullptr, "Cannot add null actor to scene.");
	m_Actors.push_back(std::unique_ptr<Actor>(actor));
	m_ActorLookup.insert({ actor->GetId(), actor });

	Camera* camera = actor->GetComponent<Camera>();
	if (camera != nullptr)
	{
		m_Cameras.push_back(camera);
	}
}
