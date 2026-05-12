#pragma once
#include "ActorComponent.h"

struct mat4x4;
struct vec3;

class TransformInterface : public ActorComponent
{
public:
	const static eComponentId COMPONENT_ID; // unique ID for this component type
	virtual eComponentId GetComponentId() const { return COMPONENT_ID; }
	virtual const mat4x4& GetTransform() const = 0;
};
