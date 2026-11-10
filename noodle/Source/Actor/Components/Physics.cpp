#include "NoodlePch.h"
#include "Physics.h"

#include "Resource/SerializationTypes.h"

#include <fstream>

const eComponentId Physics::COMPONENT_ID = eComponentId::COMPONENT_PHYSICS;

bool Physics::Init(std::ifstream& bin, EngineContext& engineContext)
{
	PhysicsSerializationData serializationData;
	N_ASSERT(bin.read(reinterpret_cast<char*>(&serializationData), sizeof(serializationData)), "Failed to read physics component serialization data.");

	m_GravityScale = serializationData.gravityScale;
	m_UsesGravity = serializationData.usesGravity;

	return true;
}
