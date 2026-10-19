#pragma once
#include "Core/NoodleCore.h"
#include "ActorComponent.h"
#include "Resource/ResourceTypes.h"

class Camera : public ActorComponent 
{
public:
	virtual void Update(float deltaSeconds) override;
	struct CameraData BuildCameraData(float32 aspectRatio) const;
	virtual eComponentId GetComponentId() const override { return COMPONENT_ID; }

	const static eComponentId COMPONENT_ID;

protected:
private:
	virtual bool Init(std::ifstream& bin, EngineContext& engineContext) override;

	vec4 m_ClearColor { 0.0f, 0.0f, 0.0f, 0.0f };
	float32 m_NearPlane = 0.0f;
	float32 m_FarPlane = 0.0f;
	float32 m_OrthoHeight = 0.0f;
	eProjectionType m_ProjectionType = eProjectionType::ORTHOGRAPHIC;
};