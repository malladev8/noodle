#pragma once
#include "Core/NoodleCore.h"
#include "CameraInterface.h"
#include "Resource/ResourceTypes.h"

class Camera : public CameraInterface 
{
public:
	virtual void Update(float deltaSeconds) override;

protected:
private:
	virtual bool Init(std::ifstream& bin, EngineContext& engineContext) override;

	vec4 m_ClearColor { 0.0f, 0.0f, 0.0f, 0.0f };
	float32 m_NearPlane = 0.0f;
	float32 m_FarPlane = 0.0f;
	float32 m_OrthoHeight = 0.0f;
	eProjectionType m_ProjectionType = eProjectionType::ORTHOGRAPHIC;
};