#pragma once
#include "Core/NoodleCore.h"
#include "ColliderData.h"
#include <vector>

class Collider;

class PhysicsSystem
{
public:
    ColliderHandle RegisterCollider(class Actor* owner, const vec3& offset, const vec3& halfExtents, float32 radius, eColliderType type);
    void UnregisterCollider(ColliderHandle handle);
    ColliderData* GetCollider(ColliderHandle handle);
    const ColliderData* GetCollider(ColliderHandle handle) const;

    void Update(float32 deltaSeconds);

protected:

private:
    struct HandleSlot
    {
        uint32 denseIndex = UINT32_MAX;
        uint32 generation = 1;
    };

    std::vector<ColliderData> m_Colliders;
    std::vector<HandleSlot> m_HandleSlots;
    std::vector<uint32> m_FreeSlots;
};