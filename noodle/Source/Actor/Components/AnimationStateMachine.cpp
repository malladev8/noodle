#include "NoodlePch.h"
#include "AnimationStateMachine.h"

#include "Actor/Actor.h"
#include "Physics.h"
#include "SpriteAnimator.h"

#include <algorithm>
#include <fstream>

const eComponentId AnimationStateMachine::COMPONENT_ID = eComponentId::COMPONENT_ANIMATION_STATE_MACHINE;

void AnimationStateMachine::PostInit(const WorldContext& worldContext)
{
	m_SpriteAnimator = GetOwner()->GetComponent<SpriteAnimator>();
	N_ASSERT(m_SpriteAnimator != nullptr, "SpriteAnimator component is required for AnimationStateMachine.");
	m_CurrentStateKey = m_DefaultStateKey;
}

void AnimationStateMachine::Update(float32 deltaSeconds)
{
	if (m_PlayingMontage)
	{
		return;
	}

	// Update Transition Condition Values for Evaluation
	Physics* physics = m_Owner->GetComponent<Physics>();

	if (physics)
	{
		SetBool("IsGrounded", physics->IsGrounded());
		SetFloat("HorizontalSpeed", abs(physics->GetVelocity().x));
		SetFloat("VerticalSpeed", physics->GetVelocity().y);
	}

	// Evaluate State Transitions
	N_ASSERT(m_AnimationStates.contains(m_CurrentStateKey), "Invalid Animation State");
	const AnimationState& currentState = m_AnimationStates[m_CurrentStateKey];
	const std::string nextStateKey = EvaluateTransitions(currentState);
	ConsumeTriggers();

	// Transition States
	m_CurrentStateKey = nextStateKey;
	N_ASSERT(m_SpriteAnimator != nullptr, "SpriteAnimator component is required for AnimationStateMachine.");
	m_SpriteAnimator->SetClipKey(m_AnimationStates[m_CurrentStateKey].clipKey);
}

void AnimationStateMachine::SetBool(const std::string& name, bool value)
{
	if (m_ConditionStateLookup.contains(name))
	{
		ConditionState& conditionState = m_ConditionStates[m_ConditionStateLookup[name]];
		conditionState.boolValue = value;
	}
}

void AnimationStateMachine::SetFloat(const std::string& name, float32 value)
{
	if (m_ConditionStateLookup.contains(name))
	{
		ConditionState& conditionState = m_ConditionStates[m_ConditionStateLookup[name]];
		conditionState.floatValue = value;
	}
}

void AnimationStateMachine::SetTrigger(const std::string& name)
{
	if (m_ConditionStateLookup.contains(name))
	{
		ConditionState& conditionState = m_ConditionStates[m_ConditionStateLookup[name]];
		conditionState.triggerValue = true;
	}
}

void AnimationStateMachine::OnClipFinished(const std::string& clipKey)
{
	if (m_PlayingMontage && clipKey == m_ActiveMontageClip)
	{
		m_PlayingMontage = false;
		m_ActiveMontageClip = "";
	}
}

void AnimationStateMachine::PlayMontage(const std::string& montageKey)
{
	if (m_PlayingMontage)
	{
		N_LOG("Cannot start new montage %s while another is playing. Current active montage: %s", m_ActiveMontageClip.c_str(), montageKey.c_str());
		return;
	}

	m_PlayingMontage = true;
	m_ActiveMontageClip = montageKey;
	N_ASSERT(m_SpriteAnimator != nullptr, "SpriteAnimator component is required for AnimationStateMachine.");
	m_SpriteAnimator->PlayClip(m_ActiveMontageClip);
}

bool AnimationStateMachine::Init(std::ifstream& bin, EngineContext& engineContext)
{
	m_DefaultStateKey = path::ReadFilePath(bin);

	uint32 numStates = 0;
	bin.read(reinterpret_cast<char*>(&numStates), sizeof(numStates));

	for (uint32 i = 0; i < numStates; ++i)
	{
		AnimationState state;
		state.name = path::ReadFilePath(bin);
		state.clipKey = path::ReadFilePath(bin);

		uint32 numTransitions = 0;
		bin.read(reinterpret_cast<char*>(&numTransitions), sizeof(numTransitions));

		for (uint32 j = 0; j < numTransitions; ++j)
		{
			AnimationTransition transition;
			transition.targetState = path::ReadFilePath(bin);

			bin.read(reinterpret_cast<char*>(&transition.priority), sizeof(transition.priority));

			uint32 numConditions = 0;
			bin.read(reinterpret_cast<char*>(&numConditions), sizeof(numConditions));

			for (uint32 k = 0; k < numConditions; ++k)
			{
				AnimationTransitionCondition condition;
				condition.name = path::ReadFilePath(bin);

				bin.read(reinterpret_cast<char*>(&condition.type), sizeof(condition.type));
				bin.read(reinterpret_cast<char*>(&condition.comparison), sizeof(condition.comparison));
				bin.read(reinterpret_cast<char*>(&condition.floatValue), sizeof(condition.floatValue));
				bin.read(reinterpret_cast<char*>(&condition.boolValue), sizeof(condition.boolValue));

				transition.conditions.push_back(condition);

				if (!m_ConditionStateLookup.contains(condition.name))
				{
					ConditionState conditionState;
					conditionState.name = condition.name;
					m_ConditionStates.push_back(conditionState);
					m_ConditionStateLookup.insert({ conditionState.name, static_cast<uint32>(m_ConditionStates.size()) - 1 });
				}
			}
			state.transitions.push_back(transition);
		}

		std::sort(state.transitions.begin(), state.transitions.end(), [](const AnimationTransition& a, const AnimationTransition& b)
			{
				return a.priority > b.priority;
			});

		m_AnimationStates.insert({ state.name, state });
	}

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
