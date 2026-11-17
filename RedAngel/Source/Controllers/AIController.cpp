#include "NoodlePch.h"
#include "AIController.h"

void AIController::Update(float deltaSeconds)
{
}

vec3 AIController::GetForwardDirection() const
{
    return m_FacingDirection == eFacingDirection::RIGHT ? vec3(1.0f, 0.0f, 0.0f) : vec3(-1.0f, 0.0f, 0.0f);
}
