#pragma once
#include "Core/NoodleCore.h"
#include "ActorComponent.h"
#include "Resource/ResourceTypes.h"

class CameraInterface : public ActorComponent
{
public:
	const static eComponentId COMPONENT_ID; // unique ID for this component type
	virtual eComponentId GetComponentId() const { return COMPONENT_ID; }
	virtual CameraData BuildCameraData(float32 aspectRatio) const = 0;
};