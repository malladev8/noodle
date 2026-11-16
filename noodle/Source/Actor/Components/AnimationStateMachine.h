#pragma once
#include "ActorComponent.h"
#include <unordered_map>
#include <vector>

enum class eAnimationTransitionConditionType : uint8
{
	BOOL = 0,
	FLOAT,
	TRIGGER
};

enum class eAnimationTransitionComparisonOperator : uint8
{
	EQUAL = 0,
	NOT_EQUAL,
	GREATER,
	LESS,
	GREATER_EQUAL,
	LESS_EQUAL
};

struct AnimationTransitionCondition
{
	std::string name = "";
	eAnimationTransitionConditionType type;
	eAnimationTransitionComparisonOperator comparison;
	float32 floatValue = 0.0f;
	bool boolValue = false;
};

struct AnimationTransition
{
	std::string targetState = "";
	std::vector<AnimationTransitionCondition> conditions;
	int32 priority = 0;
};

struct AnimationState
{
	std::string name = "";
	std::string clipKey = "";
	std::vector<AnimationTransition> transitions;
};

struct ConditionState
{
	std::string name = "";
	float32 floatValue = 0.0f;
	bool boolValue = false;
	bool triggerValue = false;
};

class AnimationStateMachine : public ActorComponent
{
public:
	const static eComponentId COMPONENT_ID;
	virtual eComponentId GetComponentId() const override { return COMPONENT_ID; }

	virtual void PostInit(const WorldContext& worldContext) override;
	virtual void Update(float32 deltaSeconds) override;

	void SetBool(const std::string& name, bool value);
	void SetFloat(const std::string& name, float32 value);
	void SetTrigger(const std::string& name);

	const std::string& GetCurrentAnimationStateName() const { return m_CurrentStateKey; }

protected:
private:
	std::unordered_map<std::string, AnimationState> m_AnimationStates;
	std::string m_CurrentStateKey = "";

	std::unordered_map<std::string, uint32> m_ConditionStateLookup;
	std::vector<ConditionState> m_ConditionStates;

	class SpriteAnimator* m_SpriteAnimator = nullptr;

	virtual bool Init(std::ifstream& bin, EngineContext& engineContext) override;
	std::string EvaluateTransitions(const AnimationState& state) const;
	bool EvaluateTransition(const AnimationTransition& transition) const;
	bool EvaluateCondition(const AnimationTransitionCondition& condition) const;
	bool EvaluateBool(const ConditionState& conditionState, const AnimationTransitionCondition& testCondition) const;
	bool EvaluateFloat(const ConditionState& conditionState, const AnimationTransitionCondition& testCondition) const;
	void ConsumeTriggers();
};