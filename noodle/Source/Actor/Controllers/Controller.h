#pragma once
#include "Core/NoodleCore.h"
class Actor;
class Controller
{
public:
	virtual ~Controller() = default;
	virtual void Update(float deltaSeconds) = 0;
	void Possess(Actor* actor);
	void UnPossess();
	Actor* GetActor() const { return m_PossessedActor; }

protected:
	Actor* m_PossessedActor = nullptr;

private:
};