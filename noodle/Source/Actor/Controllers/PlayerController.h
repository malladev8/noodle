#pragma once
#include "Core/NoodleCore.h"
#include "Controller.h"

class PlayerController : public Controller
{
public:
	PlayerController();
	virtual void Update(float deltaSeconds) override;

protected:
	class InputManager* m_InputManager;

private:

};
