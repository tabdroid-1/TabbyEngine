#pragma once

#include <Tabby/Renderer/Camera.h>
#include <Tabby/Renderer/Font.h>

#include <box2d/box2d.h>
#include <entt.hpp>

namespace Tabby {

struct IDComponent {
    UUID ID;
    bool IsPersistent = false;

    IDComponent() = default;
    IDComponent(const IDComponent&) = default;
};

struct TagComponent {
    std::string Tag;

    TagComponent() = default;
    TagComponent(const TagComponent&) = default;
    TagComponent(const std::string& tag)
        : Tag(tag)
    {
    }
};

struct HierarchyNodeComponent {
    std::pair<UUID, entt::entity> Parent = std::pair(0, entt::null);
    std::vector<std::pair<UUID, entt::entity>> Children;

    HierarchyNodeComponent() = default;
};

struct TransformComponent {
    Vector3 Translation = { 0.0f, 0.0f, 0.0f };
    Vector3 Rotation = { 0.0f, 0.0f, 0.0f };
    Vector3 Scale = { 1.0f, 1.0f, 1.0f };

    Vector3 LocalTranslation = { 0.0f, 0.0f, 0.0f };
    Vector3 LocalRotation = { 0.0f, 0.0f, 0.0f };
    Vector3 LocalScale = { 1.0f, 1.0f, 1.0f };

    Matrix4 TransformMatrix = Matrix4(1);
    Matrix4 LocalTransformMatrix = Matrix4(1);

    TransformComponent() = default;
    TransformComponent(const TransformComponent&) = default;
    TransformComponent(const Vector3& translation)
        : Translation(translation)
    {
    }

    Matrix4& GetTransform() { return TransformMatrix; }
    const Matrix4& GetTransform() const { return TransformMatrix; }
    Matrix4& GetLocalTransform() { return LocalTransformMatrix; }
    const Matrix4& GetLocalTransform() const { return LocalTransformMatrix; }
    void ApplyTransform(const Matrix4& transform);
    void ApplyTransformToLocal(const Matrix4& transform);

    operator Matrix4&() { return TransformMatrix; }
};

struct SpriteRendererComponent {
    Vector4 Color { 1.0f, 1.0f, 1.0f, 1.0f };
    AssetHandle Texture;
    int renderOrder = 0;

    int hFrames = 1;
    int vFrames = 1;
    int xFrame = 0;
    int yFrame = 0;
    float TilingFactor = 1.0f;

    SpriteRendererComponent() = default;
    SpriteRendererComponent(const SpriteRendererComponent&) = default;
    SpriteRendererComponent(const Vector4& color)
        : Color(color)
    {
    }
};

struct CircleRendererComponent {
    Vector4 Color { 1.0f, 1.0f, 1.0f, 1.0f };
    float Thickness = 1.0f;
    float Fade = 0.005f;

    int renderOrder = 0;

    CircleRendererComponent() = default;
    CircleRendererComponent(const CircleRendererComponent&) = default;
};

struct CameraComponent {
    Tabby::Camera Camera;
    bool Primary = true; // TODO: think about moving to Scene
    bool FixedAspectRatio = false;

    CameraComponent() = default;
    CameraComponent(const CameraComponent&) = default;
};

// class AudioSource;

struct SoundComponent {
    // Shared<AudioSource> Sound;
    float Gain = 1.0f;
    bool Playing = false;
    bool Loop = false;

    SoundComponent() = default;
    SoundComponent(const SoundComponent&) = default;
};

// Forward declaration
class ScriptableEntity;

struct NativeScriptComponent {
    ScriptableEntity* Instance = nullptr;

    ScriptableEntity* (*InstantiateScript)();
    void (*DestroyScript)(NativeScriptComponent*);

    template <typename T>
    void Bind()
    {
        InstantiateScript = []() {
            return static_cast<ScriptableEntity*>(new T());
        };
        DestroyScript = [](NativeScriptComponent* nsc) {
            delete nsc->Instance;
            nsc->Instance = nullptr;
        };
    }
};

// Physics

struct Rigidbody2DComponent {
    enum class BodyType { Static = 0,
        Dynamic,
        Kinematic };
    BodyType Type = BodyType::Dynamic;

    b2BodyId RuntimeBodyId = b2_nullBodyId;

    bool FixedRotation;
    bool QueuedForInitialization;

    Rigidbody2DComponent() = default;
    Rigidbody2DComponent(const Rigidbody2DComponent&) = default;

    void SetBodyType(BodyType type);
    void SetFixedRotation(bool enable);
    void SetVelocity(Vector2 velocity);
    void SetAngularVelocity(float velocity);

    BodyType GetType() const;
    Vector2 GetVelocity() const;
    Vector2 GetPosition() const;
    float GetAngularVelocity() const;
    float GetAngle() const;
};

struct BoxCollider2DComponent {
    Vector2 Offset = { 0.0f, 0.0f };
    Vector2 Size = { 0.5f, 0.5f };
    float Angle = 0.0f;

    uint32_t CollisionLayer = 1;
    uint32_t CollisionMask = 1;

    // TODO(Yan): move into physics material in the future maybe
    float Density = 1.0f;
    float Friction = 0.5f;
    float Restitution = 0.0f;
    float RestitutionThreshold = 0.5f;

    /// A sensor shape collects contact information but never generates a
    /// collision response.
    bool IsSensor = false;

    /// Enable sensor events for this shape. Only applies to kinematic and dynamic
    /// bodies. Ignored for sensors.
    bool EnableSensorEvents = false;

    /// Enable contact events for this shape. Only applies to kinematic and
    /// dynamic bodies. Ignored for sensors.
    bool EnableContactEvents = false;

    /// Enable pre-solve contact events for this shape. Only applies to dynamic
    /// bodies. These are expensive
    ///    and must be carefully handled due to multi-threading. Ignored for
    ///    sensors.
    bool EnablePreSolveEvents = false;

    // Storage for runtime
    b2ShapeId RuntimeShapeId = b2_nullShapeId;
    b2WorldId worldId = b2_nullWorldId;
    bool QueuedForInitialization;

    BoxCollider2DComponent() = default;
    BoxCollider2DComponent(const BoxCollider2DComponent&) = default;

    void SetCollisionLayer(uint32_t layer);
    uint32_t GetCollisionLayer() const;

    void SetCollisionMask(uint32_t mask);
    uint32_t GetCollisionMask() const;

    void SetCollisionLayerValue(int layerNumber, bool value);
    bool GetCollisionLayerValue(int layerNumber) const;

    void SetCollisionMaskValue(int layerNumber, bool value);
    bool GetCollisionMaskValue(int layerNumber) const;

    void RefreshShape();
};

struct CircleCollider2DComponent {
    Vector2 Offset = { 0.0f, 0.0f };
    float Radius = 0.5f;

    uint32_t CollisionLayer = 1;
    uint32_t CollisionMask = 1;

    // TODO(Yan): move into physics material in the future maybe
    float Density = 1.0f;
    float Friction = 0.5f;
    float Restitution = 0.0f;
    float RestitutionThreshold = 0.5f;

    /// A sensor shape collects contact information but never generates a
    /// collision response.
    bool IsSensor = false;

    /// Enable sensor events for this shape. Only applies to kinematic and dynamic
    /// bodies. Ignored for sensors.
    bool EnableSensorEvents = false;

    /// Enable contact events for this shape. Only applies to kinematic and
    /// dynamic bodies. Ignored for sensors.
    bool EnableContactEvents = false;

    /// Enable pre-solve contact events for this shape. Only applies to dynamic
    /// bodies. These are expensive
    ///    and must be carefully handled due to multi-threading. Ignored for
    ///    sensors.
    bool EnablePreSolveEvents = false;

    // Storage for runtime
    b2ShapeId RuntimeShapeId = b2_nullShapeId;
    b2WorldId WorldId = b2_nullWorldId;
    bool QueuedForInitialization;

    CircleCollider2DComponent() = default;
    CircleCollider2DComponent(const CircleCollider2DComponent&) = default;

    void SetCollisionLayer(uint32_t layer);
    uint32_t GetCollisionLayer() const;

    void SetCollisionMask(uint32_t mask);
    uint32_t GetCollisionMask() const;

    void SetCollisionLayerValue(int layerNumber, bool value);
    bool GetCollisionLayerValue(int layerNumber) const;

    void SetCollisionMaskValue(int layerNumber, bool value);
    bool GetCollisionMaskValue(int layerNumber) const;

    void RefreshShape();
};

struct CapsuleCollider2DComponent {
    Vector2 Offset = { 0.0f, 0.0f };
    Vector2 center1, center2;
    float Radius = 0.5f;

    uint32_t CollisionLayer = 1;
    uint32_t CollisionMask = 1;

    // TODO(Yan): move into physics material in the future maybe
    float Density = 1.0f;
    float Friction = 0.5f;
    float Restitution = 0.0f;
    float RestitutionThreshold = 0.5f;

    /// A sensor shape collects contact information but never generates a
    /// collision response.
    bool IsSensor = false;

    /// Enable sensor events for this shape. Only applies to kinematic and dynamic
    /// bodies. Ignored for sensors.
    bool EnableSensorEvents = false;

    /// Enable contact events for this shape. Only applies to kinematic and
    /// dynamic bodies. Ignored for sensors.
    bool EnableContactEvents = false;

    /// Enable pre-solve contact events for this shape. Only applies to dynamic
    /// bodies. These are expensive
    ///    and must be carefully handled due to multi-threading. Ignored for
    ///    sensors.
    bool EnablePreSolveEvents = false;

    // Storage for runtime
    b2ShapeId RuntimeShapeId = b2_nullShapeId;
    b2WorldId WorldId = b2_nullWorldId;
    bool QueuedForInitialization;

    CapsuleCollider2DComponent() = default;
    CapsuleCollider2DComponent(const CapsuleCollider2DComponent&) = default;

    void SetCollisionLayer(uint32_t layer);
    uint32_t GetCollisionLayer() const;

    void SetCollisionMask(uint32_t mask);
    uint32_t GetCollisionMask() const;

    void SetCollisionLayerValue(int layerNumber, bool value);
    bool GetCollisionLayerValue(int layerNumber) const;

    void SetCollisionMaskValue(int layerNumber, bool value);
    bool GetCollisionMaskValue(int layerNumber) const;

    void RefreshShape();
};

struct SegmentCollider2DComponent {
    Vector2 Offset = { 0.0f, 0.0f };
    Vector2 point1, point2;

    uint32_t CollisionLayer = 1;
    uint32_t CollisionMask = 1;

    // TODO(Yan): move into physics material in the future maybe
    float Density = 1.0f;
    float Friction = 0.5f;
    float Restitution = 0.0f;
    float RestitutionThreshold = 0.5f;

    /// A sensor shape collects contact information but never generates a
    /// collision response.
    bool IsSensor = false;

    /// Enable sensor events for this shape. Only applies to kinematic and dynamic
    /// bodies. Ignored for sensors.
    bool EnableSensorEvents = false;

    /// Enable contact events for this shape. Only applies to kinematic and
    /// dynamic bodies. Ignored for sensors.
    bool EnableContactEvents = false;

    /// Enable pre-solve contact events for this shape. Only applies to dynamic
    /// bodies. These are expensive
    ///    and must be carefully handled due to multi-threading. Ignored for
    ///    sensors.
    bool EnablePreSolveEvents = false;

    // Storage for runtime
    b2ShapeId RuntimeShapeId = b2_nullShapeId;
    b2WorldId WorldId = b2_nullWorldId;
    bool QueuedForInitialization;

    SegmentCollider2DComponent() = default;
    SegmentCollider2DComponent(const SegmentCollider2DComponent&) = default;

    void SetCollisionLayer(uint32_t layer);
    uint32_t GetCollisionLayer() const;

    void SetCollisionMask(uint32_t mask);
    uint32_t GetCollisionMask() const;

    void SetCollisionLayerValue(int layerNumber, bool value);
    bool GetCollisionLayerValue(int layerNumber) const;

    void SetCollisionMaskValue(int layerNumber, bool value);
    bool GetCollisionMaskValue(int layerNumber) const;

    void RefreshShape();
};

struct TextComponent {
    std::string TextString;

    Shared<Font> font = Font::GetDefault();
    // AssetHandle Font;
    Vector4 Color { 1.0f };
    float Kerning = 0.0f;
    float LineSpacing = 0.0f;

    int renderOrder = 0;
};

class Mesh;

struct MeshComponent {
    Shared<Mesh> m_Mesh;
};

template <typename... Component>
struct ComponentGroup {
};

using AllComponents = ComponentGroup<TransformComponent, SpriteRendererComponent,
    CircleRendererComponent, CameraComponent, SoundComponent, HierarchyNodeComponent, NativeScriptComponent,
    Rigidbody2DComponent, BoxCollider2DComponent, CircleCollider2DComponent, CapsuleCollider2DComponent, TextComponent>;

} // namespace Tabby
