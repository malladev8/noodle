#include "NoodlePch.h"
#include "Transform.h"
#include <fstream>

const eComponentId TransformInterface::COMPONENT_ID = eComponentId::COMPONENT_TRANSFORM;

bool Transform::Init(std::ifstream& bin)
{
    bin.read(reinterpret_cast<char*>(&m_Transform), sizeof(mat4x4));
    return true;
}