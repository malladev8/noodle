#pragma once
#include "ActorComponent.h"

class AnimationStateMachine : public ActorComponent
{
public:
	const static eComponentId COMPONENT_ID;
	virtual eComponentId GetComponentId() const override { return COMPONENT_ID; }

	virtual void Update(float32 deltaSeconds) override;

protected:
private:
	virtual bool Init(std::ifstream& bin, EngineContext& engineContext) override;
};