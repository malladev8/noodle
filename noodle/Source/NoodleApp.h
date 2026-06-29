#pragma once
#include "Core/NoodleCore.h"
#include "NoodleAllocator.h"

class Scene;
class WindowResizeEvent;
class IRenderer;
struct EngineContext;

class NoodleApp
{
public:
	NoodleApp(EngineContext& engineContext);
	virtual ~NoodleApp();

	virtual void Initialize();
	virtual void Update(float deltaSeconds);
	virtual void Render(IRenderer& IRenderer);
	virtual void Shutdown();

protected:
	std::unique_ptr<Scene> m_Scene;
	EngineContext& m_EngineContext;

private:
};