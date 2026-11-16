#include "NoodlePch.h"
#include "AnimationStateMachine.h"

#include "Actor/Actor.h"
#include "Physics.h"
#include "SpriteAnimator.h"

void AnimationStateMachine::PostInit(const WorldContext& worldContext)
{
	m_SpriteAnimator = GetOwner()->GetComponent<SpriteAnimator>();
	N_ASSERT(m_SpriteAnimator != nullptr, "SpriteAnimator component is required for AnimationStateMachine.");
}

void AnimationStateMachine::Update(float32 deltaSeconds)
{
	// Update Transition Condition Values for Evaluation
	Physics* physics = m_Owner->GetComponent<Physics>();

	if (physics)
	{
		SetBool("IsGrounded", physics->IsGrounded());
		SetFloat("HorizontalSpeed", physics->GetVelocity().x);
		SetFloat("VerticalSpeed", physics->GetVelocity().y);
	}

	// Evaluate State Transitions
	N_ASSERT(m_AnimationStates.contains(m_CurrentStateKey), "Invalid Animation State");
	AnimationState& currentState = m_AnimationStates[m_CurrentStateKey];
	std::string nextStateKey = EvaluateTransitions(currentState);
	ConsumeTriggers();

	// Transition States
	m_CurrentStateKey = nextStateKey;
	N_ASSERT(m_SpriteAnimator != nullptr, "SpriteAnimator component is required for AnimationStateMachine.");
	m_SpriteAnimator->SetClipKey(m_AnimationStates[m_CurrentStateKey].clipKey);
}

void AnimationStateMachine::SetBool(const std::string& name, bool value)
{
	if (!m_ConditionStateLookup.contains(name))
	{
		N_LOG("Invalid Animation Transition Condition Key: %s", name.c_str());
		return;
	}
	ConditionState& conditionState = m_ConditionStates[m_ConditionStateLookup[name]];
	conditionState.boolValue = value;
}

void AnimationStateMachine::SetFloat(const std::string& name, float32 value)
{
	if (!m_ConditionStateLookup.contains(name))
	{
		N_LOG("Invalid Animation Transition Condition Key: %s", name.c_str());
		return;
	}
	ConditionState& conditionState = m_ConditionStates[m_ConditionStateLookup[name]];
	conditionState.floatValue = value;
}

void AnimationStateMachine::SetTrigger(const std::string& name)
{
	if (!m_ConditionStateLookup.contains(name))
	{
		N_LOG("Invalid Animation Transition Condition Key: %s", name.c_str());
		return;
	}
	ConditionState& conditionState = m_ConditionStates[m_ConditionStateLookup[name]];
	conditionState.triggerValue = true;
}

bool AnimationStateMachine::Init(std::ifstream& bin, EngineContext& engineContext)
{
	return true;
}

std::string AnimationStateMachine::EvaluateTransitions(const AnimationState& state) const
{
	for (const AnimationTransition& transition : state.transitions)
	{
		if (EvaluateTransition(transition))
		{
			return transition.targetState;
		}
	}
	return m_CurrentStateKey;
}

bool AnimationStateMachine::EvaluateTransition(const AnimationTransition& transition) const
{
	for (const AnimationTransitionCondition& condition : transition.conditions)
	{
		if (!EvaluateCondition(condition))
		{
			return false;
		}
	}
	return true;
}

bool AnimationStateMachine::EvaluateCondition(const AnimationTransitionCondition& condition) const
{
	const ConditionState& conditionState = m_ConditionStates[m_ConditionStateLookup.at(condition.name)];
	switch (condition.type)	
	{
	default:
		return false;
	case eAnimationTransitionConditionType::BOOL:
	{
		return EvaluateBool(conditionState, condition);
	}
	case eAnimationTransitionConditionType::FLOAT:
	{
		return EvaluateFloat(conditionState, condition);
	}
	case eAnimationTransitionConditionType::TRIGGER:
	{
		return conditionState.triggerValue;
	}
	}
	return false;
}

bool AnimationStateMachine::EvaluateBool(const ConditionState& conditionState, const AnimationTransitionCondition& testCondition) const
{
	if (testCondition.comparison == eAnimationTransitionComparisonOperator::EQUAL)
	{
		return testCondition.boolValue == conditionState.boolValue;
	}
	else if(testCondition.comparison == eAnimationTransitionComparisonOperator::NOT_EQUAL)
	{
		return testCondition.boolValue != conditionState.boolValue;
	}
	N_LOG("Unsupported Comparison for Animation State Machine boolean condition: %i", static_cast<uint8>(testCondition.comparison));
	return false;
}

bool AnimationStateMachine::EvaluateFloat(const ConditionState& conditionState, const AnimationTransitionCondition& testCondition) const
{
	switch (testCondition.comparison)
	{
	default:
		return false;
	case eAnimationTransitionComparisonOperator::EQUAL:
	{
		return abs(conditionState.floatValue - testCondition.floatValue) <= EPSILON;
	}
	case eAnimationTransitionComparisonOperator::NOT_EQUAL:
	{
		return abs(conditionState.floatValue - testCondition.floatValue) > EPSILON;
	}
	case eAnimationTransitionComparisonOperator::GREATER:
	{
		if (abs(conditionState.floatValue - testCondition.floatValue) > EPSILON &&
			conditionState.floatValue > testCondition.floatValue)
		{
			return true;
		}
		return false;
	}
	case eAnimationTransitionComparisonOperator::LESS:
	{
		if (abs(conditionState.floatValue - testCondition.floatValue) > EPSILON &&
			conditionState.floatValue < testCondition.floatValue)
		{
			return true;
		}
		return false;
	}
	case eAnimationTransitionComparisonOperator::GREATER_EQUAL:
	{
		if (abs(conditionState.floatValue - testCondition.floatValue) <= EPSILON ||
			conditionState.floatValue > testCondition.floatValue)
		{
			return true;
		}
		return false;
	}
	case eAnimationTransitionComparisonOperator::LESS_EQUAL:
	{
		if (abs(conditionState.floatValue - testCondition.floatValue) <= EPSILON ||
			conditionState.floatValue < testCondition.floatValue)
		{
			return true;
		}
		return false;
	}
	}
	return false;
}

void AnimationStateMachine::ConsumeTriggers()
{
	for (ConditionState& condition : m_ConditionStates)
	{
		condition.triggerValue = false;
	}
}
