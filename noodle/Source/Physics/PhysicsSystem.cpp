#include "NoodlePch.h"
#include "PhysicsSystem.h"
#include "Actor/Actor.h"
#include "Actor/Components/Transform.h"

ColliderHandle PhysicsSystem::RegisterCollider(Actor* owner, const vec3& offset, const vec3& halfExtents, float32 radius, eColliderType type)
{
    uint32 slotIndex;

    if (!m_FreeSlots.empty())
    {
        slotIndex = m_FreeSlots.back();
        m_FreeSlots.pop_back();
    }
    else
    {
        slotIndex = static_cast<uint32>(m_HandleSlots.size());
        m_HandleSlots.emplace_back();
    }

    ColliderData data;
    data.owner = owner;
    data.offset = offset;
    data.halfExtents = halfExtents;
    data.radius = radius;
    data.type = type;
    data.handleSlot = slotIndex;

    const uint32 denseIndex = static_cast<uint32>(m_Colliders.size());

    m_Colliders.push_back(data);
    m_HandleSlots[slotIndex].denseIndex = denseIndex;

    return { slotIndex, m_HandleSlots[slotIndex].generation };
}

void PhysicsSystem::UnregisterCollider(ColliderHandle handle)
{
    if (!handle.IsValid() || handle.index >= m_HandleSlots.size())
    {
        return;
    }

    HandleSlot& slot = m_HandleSlots[handle.index];

    if (slot.generation != handle.generation || slot.denseIndex == UINT32_MAX)
    {
        return; // Stale or already-removed handle.
    }

    const uint32 denseIndex = slot.denseIndex;
    const uint32 lastIndex = static_cast<uint32>(m_Colliders.size() - 1);

    if (denseIndex != lastIndex)
    {
        // Move the final collider into the removed entry's position.
        m_Colliders[denseIndex] = std::move(m_Colliders[lastIndex]);

        // Find the handle slot belonging to the moved collider.
        const uint32 movedSlot = m_Colliders[denseIndex].handleSlot;
        m_HandleSlots[movedSlot].denseIndex = denseIndex;
    }

    m_Colliders.pop_back();
     
    slot.denseIndex = UINT32_MAX;
    ++slot.generation;
    m_FreeSlots.push_back(handle.index);
}

ColliderData* PhysicsSystem::GetCollider(ColliderHandle handle)
{
    if (!handle.IsValid() || handle.index >= m_HandleSlots.size())
    {
        return nullptr;
    }

    const HandleSlot& slot = m_HandleSlots[handle.index];

    if (slot.generation != handle.generation || slot.denseIndex == UINT32_MAX)
    {
        return nullptr;
    }

    N_ASSERT(slot.denseIndex < m_Colliders.size(), "Collider handle maps to an invalid dense index.");

    return &m_Colliders[slot.denseIndex];
}

const ColliderData* PhysicsSystem::GetCollider(ColliderHandle handle) const
{
    if (!handle.IsValid() || handle.index >= m_HandleSlots.size())
    {
        return nullptr;
    }

    const HandleSlot& slot = m_HandleSlots[handle.index];

    if (slot.generation != handle.generation || slot.denseIndex == UINT32_MAX)
    {
        return nullptr;
    }

    N_ASSERT(slot.denseIndex < m_Colliders.size(), "Collider handle maps to an invalid dense index.");

    return &m_Colliders[slot.denseIndex];
}

static AABB CalculateWorldAABB(const ColliderData& collider)
{
    N_ASSERT(collider.owner != nullptr, "Collider owning Actor is null.");

    Transform* transform = collider.owner->GetComponent<Transform>();

    N_ASSERT(transform != nullptr, "Collider owning Actor is missing Transform component.");

    const vec3& pos = transform->GetPosition();
    const vec3 center = { pos.x + collider.offset.x, pos.y + collider.offset.y, pos.z + collider.offset.z };

    const vec3& scale = transform->GetScale();

    const vec3 halfExtents = { collider.halfExtents.x * scale.x, collider.halfExtents.y * scale.y, collider.halfExtents.z * scale.z };

    AABB aabb;
    aabb.min = { center.x - halfExtents.x, center.y - halfExtents.y, center.z - halfExtents.z };
    aabb.max = { center.x + halfExtents.x, center.y + halfExtents.y, center.z + halfExtents.z };

    return aabb;
}

void PhysicsSystem::Update(float32 deltaSeconds)
{
    for (ColliderData& collider : m_Colliders)
    {
        collider.worldAABB = CalculateWorldAABB(collider);
    }

    // TODO: broadphase, overlap tests, etc.
}