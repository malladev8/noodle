#pragma once
#include "Core/NoodleCore.h"
#include "TransformInterface.h"
#include "Math/NoodleMath.h"

class Transform : public TransformInterface
{
public:
	const vec3& GetPosition() const { return m_Position; }
	const vec3& GetScale() const { return m_Scale; }
	const quaternion& GetRotation() const { return m_Rotation; }
	mat4x4 GetWorldMatrix() const;
	mat4x4 GetLocalMatrix() const;

	void SetPosition(const vec3& pos) { m_Position = pos; }
	void SetScale(const vec3& scale) { m_Scale = scale; }
	void SetRotation(const quaternion& rot) { m_Rotation = rot; }

	void Translate(const vec3& translation);

protected:
private:
	vec3 m_Position;
	vec3 m_Scale;
	quaternion m_Rotation;

	virtual bool Init(std::ifstream& bin, EngineContext& engineContext) override;
};