#pragma once
#include "Core/NoodleCore.h"
#include "NoodleAllocator.h"

class Scene;
class WindowResizeEvent;

class NoodleApp
{
public:
	NoodleApp();
	virtual ~NoodleApp();

	virtual void Init();
	virtual void Run(float deltaSeconds);

	void OnWindowResize(const WindowResizeEvent& event);

protected:
	std::unique_ptr<Scene> m_Scene;
private:
	uint32 m_WindowResizeEventHandle = 0;
};