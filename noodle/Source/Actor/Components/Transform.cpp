#include "NoodlePch.h"
#include "Transform.h"
#include "Actor/Actor.h"
#include "EngineContext.h"
#include <fstream>

const eComponentId TransformInterface::COMPONENT_ID = eComponentId::COMPONENT_TRANSFORM;

mat4x4 Transform::GetWorldMatrix() const
{
    const mat4x4 local = GetLocalMatrix();
    const Actor* parent = m_Owner->GetParent();
    if (parent)
    {
        return parent->GetTransform()->GetWorldMatrix() * local;
    }
    return local;
}

mat4x4 Transform::GetLocalMatrix() const
{
    return mat4x4::BuildTranslation(m_Position) * mat4x4::BuildRotation(m_Rotation) * mat4x4::BuildScale(m_Scale);
}

void Transform::Translate(const vec3& translation)
{
    m_Position += translation;
}

bool Transform::Init(std::ifstream& bin, EngineContext& engineContext)
{
    bin.read(reinterpret_cast<char*>(&m_Position), sizeof(vec3));
    bin.read(reinterpret_cast<char*>(&m_Scale), sizeof(vec3));
    bin.read(reinterpret_cast<char*>(&m_Rotation), sizeof(quaternion));
    return true;
}