#pragma once

#include <Tabby/World/Entity.h>
#include <box2d/box2d.h>

namespace Tabby {

struct TransformComponent;
struct Rigidbody2DComponent;

struct RaycastFilter2D {
public:
    RaycastFilter2D()
        : collisionLayer(0x00000001)
        , collisionMask(0xFFFFFFFF)
    {
    }

    RaycastFilter2D(uint32_t layer, uint32_t mask)
        : collisionLayer(layer)
        , collisionMask(mask)
    {
    }

    void SetCollisionLayer(uint32_t layer);
    uint32_t GetCollisionLayer() const;

    void SetCollisionMask(uint32_t layer);
    uint32_t GetCollisionMask() const;

    void SetLayerValue(int layerNumber, bool value);
    bool GetLayerValue(int layerNumber) const;

    void SetMaskValue(int layerNumber, bool value);
    bool GetMaskValue(int layerNumber) const;

private:
    uint32_t collisionLayer = 1;
    uint32_t collisionMask = 1;
};

enum ColliderType2D : uint8_t {
    Box = 0,
    Circle,
    Capsule,
    Segment
};

struct ShapeUserData2D {
    Entity shapeEntity;
    Entity bodyEntity;
    ColliderType2D colliderType;
};

struct BodyUserData2D {
    Entity bodyEntity;
    std::unordered_map<UUID, Entity> shapeEntities;
};

struct ContactCallback {
    Entity entity;
    TransformComponent* transform;
    Rigidbody2DComponent* rigidbody;
};

struct BodyInfo2D {
    Entity entity;
};

struct ShapeInfo2D {
    Entity entity;
    ColliderType2D colliderType;
};

struct RaycastHit2D {
    RaycastFilter2D RaycastFilter;
    Entity entity;
    TransformComponent* transform;
    Rigidbody2DComponent* rigidbody;
    Vector2 origin = { 0.0f, 0.0f };
    Vector2 point = { 0.0f, 0.0f };
    Vector2 normal = { 0.0f, 0.0f };
    float distance;
    float fraction;

    operator bool() { return entity.Valid(); }
};

}
