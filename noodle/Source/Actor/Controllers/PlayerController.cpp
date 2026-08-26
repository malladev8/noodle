#include "NoodlePch.h"
#include "PlayerController.h"
#include "NoodleEngine.h"
#include "NoodleInput.h"

PlayerController::PlayerController()
{
	m_InputManager = &Engine::Get().GetContext().inputManager;
}

void PlayerController::Update(float deltaSeconds)
{
}
