#pragma once
#include "Core/NoodleCore.h"
#include "NoodleAllocator.h"

class Scene;
class WindowResizeEvent;
struct EngineContext;

class NoodleApp
{
public:
	NoodleApp(EngineContext& engineContext);
	virtual ~NoodleApp();

	virtual void Init();
	virtual void Run(float deltaSeconds);
	virtual void Shutdown();

	virtual void OnWindowResize(const WindowResizeEvent& event);

protected:
	std::unique_ptr<Scene> m_Scene;
	EngineContext& m_EngineContext;
	uint32 m_WindowResizeEventHandle = 0;

private:
};