#pragma once
#include "Resource/ResourceTypes.h"

struct ColliderHandle
{
    uint32 index = UINT32_MAX;
    uint32 generation = 0;

    bool IsValid() const { return index != UINT32_MAX; }
};

struct ColliderData
{
    class Actor* owner = nullptr;

    vec3 offset = {};
    vec3 halfExtents = { 0.5f, 0.5f, 0.0f };
    float32 radius = 0.5f;
    uint32 handleSlot = UINT32_MAX;
    eColliderType type = eColliderType::AABB;

    AABB worldAABB = {};
};