#pragma once
#include "Core/NoodleCore.h"
#include "Actor/Controllers/Controller.h"

class PlayerController : public Controller
{
public:
	PlayerController();
	virtual void Update(float deltaSeconds) override;
protected:
private:
};
