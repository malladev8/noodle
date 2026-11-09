#include "NoodlePch.h"
#include "PhysicsSystem2D.h"
#include "Actor/Actor.h"
#include "Actor/Components/Collider.h"
#include "Actor/Components/Transform.h"

void PhysicsSystem2D::Update(float32 deltaSeconds)
{
    ProcessMovementRequests();
    UpdateDynamicBounds();
    DetectDynamicDynamicOverlaps();
    m_MovementRequests.clear();
}

void PhysicsSystem2D::ProcessMovementRequests()
{
    for (const MovementRequest& request : m_MovementRequests)
    {
        ColliderData* collider = GetCollider(request.colliderHandle);

        if (collider == nullptr || collider->mobility != eColliderMobility::DYNAMIC)
        {
            continue;
        }

        Transform* transform = collider->owner->GetComponent<Transform>();

        vec3 position = transform->GetPosition();

        // X-Axis movement.
        const float32 dx = ResolveAxis(*collider, request.displacement.x, 0);
        position.x += dx;
        transform->SetPosition(position);
        collider->worldAABB = sCalculateWorldAABB(*collider);

        // Y-Axis movement.
        const float32 dy = ResolveAxis(*collider, request.displacement.y, 1);
        position = transform->GetPosition();
        position.y += dy;
        transform->SetPosition(position);
        collider->worldAABB = sCalculateWorldAABB(*collider);
    }
}

void PhysicsSystem2D::UpdateDynamicBounds()
{
    for (ColliderData& collider : m_Colliders[DYNAMIC_COLLIDER_INDEX])
    {
        collider.worldAABB = sCalculateWorldAABB(collider);
    }
}

void PhysicsSystem2D::DetectDynamicDynamicOverlaps()
{
    // Test each unique pair once.
    const size_t count = m_Colliders[DYNAMIC_COLLIDER_INDEX].size();

    for (size_t i = 0; i < count; ++i)
    {
        for (size_t j = i + 1; j < count; ++j)
        {
            ColliderData& a = m_Colliders[DYNAMIC_COLLIDER_INDEX][i];
            ColliderData& b = m_Colliders[DYNAMIC_COLLIDER_INDEX][j];

            // TODO: Narrowphase - Perform appropriate collider type specific tests

            if (sCheckAABBCollision(a.worldAABB, b.worldAABB))
            {
                // TODO: Handle overlap.
            }
        }
    }
}

float32 PhysicsSystem2D::ResolveAxis(const ColliderData& dynamicCollider, float32 displacement, uint32 axis) const
{
    if (displacement == 0.0f)
    {
        return 0.0f;
    }

    float32 resolvedDisplacement = displacement;

    const AABB& bounds = dynamicCollider.worldAABB;

    for (const ColliderData& staticCollider : m_Colliders[STATIC_COLLIDER_INDEX])
    {
        if (!sCheckAABBCollisionOnOtherAxes(bounds, staticCollider.worldAABB, axis))
        {
            continue;
        }

        const AABB& obstacle = staticCollider.worldAABB;

        if (displacement > 0.0f)
        {
            const float32 distance = obstacle.min[axis] - bounds.max[axis];
            if (distance >= 0.0f && distance < resolvedDisplacement)
            {
                resolvedDisplacement = distance;
            }
        }
        else
        {
            const float32 distance = obstacle.max[axis] - bounds.min[axis];
            if (distance <= 0.0f && distance > resolvedDisplacement)
            {
                resolvedDisplacement = distance;
            }
        }
    }

    return resolvedDisplacement;
}

bool PhysicsSystem2D::sCheckAABBCollisionOnOtherAxes(const AABB& a, const AABB& b, uint32 axis)
{
    const uint32 numAxes = 2;
    for (uint32 i = 0; i < numAxes; ++i)
    {
        if (i == axis)
        {
            continue;
        }

        if (a.min[i] >= b.max[i] || a.max[i] <= b.min[i])
        {
            return false;
        }
    }

    return true;
}