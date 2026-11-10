#pragma once
#include "Core/NoodleCore.h"
#include "ColliderData.h"
#include <array>
#include <vector>

class Actor;
class Collider;

static constexpr uint32 STATIC_COLLIDER_INDEX = static_cast<uint32>(eColliderMobility::STATIC);
static constexpr uint32 DYNAMIC_COLLIDER_INDEX = static_cast<uint32>(eColliderMobility::DYNAMIC);
static const vec3 GRAVITY_ACCELERATION { 0.0f, -9.81f, 0.0f };

enum class ePhysicsDimension : uint8
{
    TWO_D = 0,
    THREE_D
};

struct ForceRequest
{
    ColliderHandle colliderHandle;
    vec3 force;
};

struct ImpulseRequest
{
    ColliderHandle colliderHandle;
    vec3 impulse;
};

class PhysicsSystem
{
public:
    virtual ~PhysicsSystem() {}
    ColliderHandle RegisterCollider(class Actor* owner, const vec3& offset, const vec3& halfExtents, float32 radius, eColliderType type, eColliderMobility mobility);
    void UnregisterCollider(ColliderHandle handle);
    ColliderData* GetCollider(ColliderHandle handle);
    const ColliderData* GetCollider(ColliderHandle handle) const;

    virtual void Update(float32 deltaSeconds) = 0;
    void SubmitDebugRenderCommands(class IRenderer& renderer) const;

    // Call every frame while force should be active
    void RequestForce(ColliderHandle handle, const vec3& force);

    // Call once to apply an immediate impulse
    void RequestImpulse(ColliderHandle handle, const vec3& impulse);

    static AABB sCalculateWorldAABB(const ColliderData& collider);
    static bool sCheckAABBCollision(const AABB& a, const AABB& b);

protected:
    struct HandleSlot
    {
        uint32 denseIndex = UINT32_MAX;
        uint32 generation = 1;
        eColliderMobility mobility = eColliderMobility::STATIC;
    };

    std::array<std::vector<ColliderData>, static_cast<uint32>(eColliderMobility::COUNT)> m_Colliders;
    std::vector<HandleSlot> m_HandleSlots;
    std::vector<uint32> m_FreeSlots;

    std::vector<ForceRequest> m_ForceRequests;
    std::vector<ImpulseRequest> m_ImpulseRequests;

private:
};