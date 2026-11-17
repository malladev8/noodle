#pragma once
#include "Core/NoodleCore.h"
#include "Actor/Controllers/Controller.h"

class AIController : public Controller
{
public:
	virtual void Update(float deltaSeconds) override;
	virtual vec3 GetForwardDirection() const override;

protected:
private:
};
