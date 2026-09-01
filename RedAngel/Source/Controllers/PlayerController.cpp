#include "NoodlePch.h"
#include "PlayerController.h"
#include "NoodleEngine.h"
#include "NoodleInput.h"
#include "Actor/Actor.h"
#include "Actor/Components/Movement.h"

PlayerController::PlayerController()
{
}

void PlayerController::Update(float deltaSeconds)
{
	const InputManager& inputManager = Engine::Get().GetContext().inputManager;

	vec3 movementInput;
	if (inputManager.IsKeyDown(eKey::W))
	{
		movementInput.y += 1.0f;
	}
	if (inputManager.IsKeyDown(eKey::A))
	{
		movementInput.x -= 1.0f;
	}
	if (inputManager.IsKeyDown(eKey::S))
	{
		movementInput.y -= 1.0f;
	}
	if (inputManager.IsKeyDown(eKey::D))
	{
		movementInput.x += 1.0f;
	}
	
	Movement* movementComponent = m_PossessedActor->GetComponent<Movement>();
	if (movementComponent)
	{
		movementComponent->SetMoveInput(movementInput);
	}
}
