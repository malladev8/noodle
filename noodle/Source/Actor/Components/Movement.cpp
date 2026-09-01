#include "NoodlePch.h"
#include "Movement.h"
#include <fstream>

const eComponentId MovementInterface::COMPONENT_ID = eComponentId::COMPONENT_MOVEMENT;

void Movement::Update(float32 deltaSeconds)
{
}

bool Movement::Init(std::ifstream& bin, EngineContext& engineContext)
{
	bin.read(reinterpret_cast<char*>(&m_Speed), sizeof(float32));
	return true;
}
