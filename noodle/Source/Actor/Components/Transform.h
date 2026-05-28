#pragma once
#include "Core/NoodleCore.h"
#include "TransformInterface.h"
#include "Math/NoodleMath.h"

class Transform : public TransformInterface
{
public:
	virtual const mat4x4& GetTransform() const override { return m_Transform; };

protected:
private:
	mat4x4 m_Transform;

	virtual bool Init(std::ifstream& bin) override;
};