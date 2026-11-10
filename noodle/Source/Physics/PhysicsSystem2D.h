#pragma once
#include "PhysicsSystem.h"

class PhysicsSystem2D : public PhysicsSystem
{
public:
	virtual void Update(float32 deltaSeconds) override;

protected:

private:
    void IntegrateBodies(float32 deltaSeconds);
    void UpdateDynamicBounds();
    void DetectDynamicDynamicOverlaps();
    float32 ResolveAxis(const ColliderData& dynamicCollider, float32 displacement, uint32 axis) const;

    static bool sCheckAABBCollisionOnOtherAxes(const AABB& a, const AABB& b, uint32 axis);
};