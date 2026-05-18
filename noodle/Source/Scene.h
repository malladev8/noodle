#pragma once
#include "Core/NoodleCore.h"
#include <vector>

class Actor;

class Scene
{
public:
	void Update(int32 deltaMs);
protected:
private:
	std::vector<std::unique_ptr<Actor>> m_Actors;
};