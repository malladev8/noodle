#pragma once
#include "Core/NoodleCore.h"
#include "ColliderData.h"
#include <array>
#include <vector>

class Actor;
class Collider;

static constexpr uint32 STATIC_COLLIDER_INDEX = static_cast<uint32>(eColliderMobility::STATIC);
static constexpr uint32 DYNAMIC_COLLIDER_INDEX = static_cast<uint32>(eColliderMobility::DYNAMIC);

struct MovementRequest
{
    ColliderHandle colliderHandle;
    vec3 displacement;
};

class PhysicsSystem
{
public:
    ColliderHandle RegisterCollider(class Actor* owner, const vec3& offset, const vec3& halfExtents, float32 radius, eColliderType type, eColliderMobility mobility);
    void UnregisterCollider(ColliderHandle handle);
    ColliderData* GetCollider(ColliderHandle handle);
    const ColliderData* GetCollider(ColliderHandle handle) const;

    void Update(float32 deltaSeconds);
    void SubmitDebugRenderCommands(class IRenderer& renderer) const;

    void RequestMovement(ColliderHandle colliderHandle, const vec3& displacement);

    static AABB sCalculateWorldAABB(const ColliderData& collider);
    static bool sCheckAABBCollision(const AABB& a, const AABB& b);

protected:

private:
    struct HandleSlot
    {
        uint32 denseIndex = UINT32_MAX;
        uint32 generation = 1;
        eColliderMobility mobility = eColliderMobility::STATIC;
    };

    std::array<std::vector<ColliderData>, static_cast<uint32>(eColliderMobility::COUNT)> m_Colliders;
    std::vector<HandleSlot> m_HandleSlots;
    std::vector<uint32> m_FreeSlots;
    std::vector<MovementRequest> m_MovementRequests;

    void ProcessMovementRequests();
    void UpdateDynamicBounds();
    void DetectDynamicDynamicOverlaps();
    float32 ResolveAxis(const ColliderData& dynamicCollider, float32 displacement, uint32 axis) const;

    static bool sCheckAABBCollisionOnOtherAxes(const AABB& a, const AABB& b, uint32 axis);
};