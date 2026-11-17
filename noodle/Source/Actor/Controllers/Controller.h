#pragma once
#include "Core/NoodleCore.h"
#include "Math/NoodleMath.h"

enum class eFacingDirection : uint8
{
	LEFT = 0,
	RIGHT
};

class Actor;
class Controller
{
public:
	virtual ~Controller() = default;
	virtual void Update(float deltaSeconds) = 0;
	void Possess(Actor* actor);
	void UnPossess();
	Actor* const GetActor() const { return m_PossessedActor; }

	void SetFacingDirection(eFacingDirection direction) { m_FacingDirection = direction; }
	eFacingDirection GetFacingDirection() const { return m_FacingDirection; }
	virtual vec3 GetForwardDirection() const = 0;

protected:
	Actor* m_PossessedActor = nullptr;
	eFacingDirection m_FacingDirection = eFacingDirection::RIGHT;

private:
};