#include "NoodlePch.h"
#include "PhysicsSystem2D.h"
#include "Actor/Actor.h"
#include "Actor/Components/Collider.h"
#include "Actor/Components/Physics.h"
#include "Actor/Components/Transform.h"

void PhysicsSystem2D::Update(float32 deltaSeconds)
{
    // TODO: Aggregate forces and impulses by collider handle before integrating bodies to avoid O(nm) force and impulse integration in IntegrateBodies()
    IntegrateBodies(deltaSeconds);
    UpdateDynamicBounds();
    DetectDynamicDynamicOverlaps();
    m_ForceRequests.clear();
    m_ImpulseRequests.clear();
}

void PhysicsSystem2D::IntegrateBodies(float32 deltaSeconds)
{
    for (ColliderData& collider : m_Colliders[DYNAMIC_COLLIDER_INDEX])
    {
        Physics* physicsComponent = collider.owner->GetComponent<Physics>();
        N_ASSERT(physicsComponent != nullptr, "Actors with Dynamic Colliders require Physics component.");

        Transform* transform = collider.owner->GetComponent<Transform>();
        N_ASSERT(transform != nullptr, "Dynamic Collider owner is missing Transform component.");

        vec3 velocity = physicsComponent->GetVelocity();

        // Apply accumulated impulses.
        vec3 totalImpulse = {};

        for (const ImpulseRequest& request : m_ImpulseRequests)
        {
            if (GetCollider(request.colliderHandle) == &collider)
            {
                totalImpulse += request.impulse;
            }
        }

        velocity += totalImpulse * (1 / physicsComponent->GetMass());

        // Accumulate forces.
        vec3 totalForce = {};

        for (const ForceRequest& request : m_ForceRequests)
        {
            if (GetCollider(request.colliderHandle) == &collider)
            {
                totalForce += request.force;
            }
        }
        vec3 acceleration = totalForce * (1 / physicsComponent->GetMass());

        // Apply gravity as acceleration.
        if (physicsComponent->IsAffectedByGravity())
        {
            acceleration += GRAVITY_ACCELERATION * physicsComponent->GetGravityScale();
        }

        // Semi-implicit Euler integration.
        velocity += acceleration * deltaSeconds;

        // Calculate requested displacement.
        const vec3 displacement = velocity * deltaSeconds;

        vec3 position = transform->GetPosition();

        // X-axis resolution.
        const float32 requestedX = displacement.x;
        const float32 resolvedX = ResolveAxis(collider, requestedX, 0);

        position.x += resolvedX;

        transform->SetPosition(position);
        collider.worldAABB = sCalculateWorldAABB(collider);

        if (std::abs(resolvedX - requestedX) > EPSILON)
        {
            velocity.x = 0.0f;
        }

        // Y-axis resolution.
        const float32 requestedY = displacement.y;
        const float32 resolvedY = ResolveAxis(collider, requestedY, 1);

        position.y += resolvedY;

        transform->SetPosition(position);
        collider.worldAABB = sCalculateWorldAABB(collider);

        physicsComponent->m_IsGrounded = false;

        if (std::abs(resolvedY - requestedY) > EPSILON)
        {
            if (requestedY < 0.0f)
            {
                physicsComponent->m_IsGrounded = true;
            }

            velocity.y = 0.0f;
        }

        physicsComponent->SetVelocity(velocity);
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