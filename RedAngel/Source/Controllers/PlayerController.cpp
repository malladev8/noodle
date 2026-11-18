#include "NoodlePch.h"
#include "PlayerController.h"
#include "NoodleEngine.h"
#include "NoodleInput.h"
#include "Actor/Actor.h"
#include "Actor/Components/AnimationStateMachine.h"
#include "Actor/Components/Movement.h"
#include "Actor/Components/Sprite.h"

PlayerController::PlayerController()
{
}

void PlayerController::Update(float deltaSeconds)
{
	const InputManager& inputManager = Engine::Get().GetContext().inputManager;

	// Movement testing
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

	Sprite* spriteComponent = m_PossessedActor->GetComponent<Sprite>();
	if (movementInput.x > 0.0f)
	{
		SetFacingDirection(eFacingDirection::RIGHT);
		if (spriteComponent)
		{
			spriteComponent->SetFlipX(false);
		}
	}
	else if (movementInput.x < 0.0f)
	{
		SetFacingDirection(eFacingDirection::LEFT);
		if (spriteComponent)
		{
			spriteComponent->SetFlipX(true);
		}
	}

	bool jump = false;
	if (inputManager.IsKeyDown(eKey::SPACE))
	{
		jump = true;
	}
	
	Movement* movementComponent = m_PossessedActor->GetComponent<Movement>();
	if (movementComponent)
	{
		movementComponent->SetMoveInput(movementInput);
		movementComponent->SetJump(jump);
	}

	// Attack testing
	if (inputManager.WasKeyPressed(eKey::MOUSE_L))
	{
		AnimationStateMachine* animStateMachine = m_PossessedActor->GetComponent<AnimationStateMachine>();
		if (animStateMachine)
		{
			animStateMachine->PlayMontage("Attack_1");
		}
	}
}

vec3 PlayerController::GetForwardDirection() const
{
	return m_FacingDirection == eFacingDirection::RIGHT ? vec3(1.0f, 0.0f, 0.0f) : vec3(-1.0f, 0.0f, 0.0f);
}
