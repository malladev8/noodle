#include "NoodlePch.h"
#include "Transform.h"
#include "EngineContext.h"
#include <fstream>

const eComponentId TransformInterface::COMPONENT_ID = eComponentId::COMPONENT_TRANSFORM;

mat4x4 Transform::GetWorldMatrix() const
{
    return mat4x4::BuildTranslation(m_Position) * mat4x4::BuildRotation(m_Rotation) * mat4x4::BuildScale(m_Scale);
}

bool Transform::Init(std::ifstream& bin, EngineContext& engineContext)
{
    bin.read(reinterpret_cast<char*>(&m_Position), sizeof(vec3));
    bin.read(reinterpret_cast<char*>(&m_Scale), sizeof(vec3));
    bin.read(reinterpret_cast<char*>(&m_Rotation), sizeof(quaternion));
    return true;
}