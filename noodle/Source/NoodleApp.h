#pragma once
#include "Core/NoodleCore.h"
#include "NoodleAllocator.h"

class Scene;

class NoodleApp
{
public:
	NoodleApp();
	virtual ~NoodleApp();

	virtual void Init();
	virtual void Run(float deltaSeconds);

protected:
	std::unique_ptr<Scene> m_Scene;
private:
};