#include <Tabby/World/Components.h>
#include <Tabby/Physics/2D/Physics2D.h>
#include <Tabby/Physics/2D/Physics2DTypes.h>
#include <Tabby/Physics/2D/Physics2DUtil.h>
#include <Tabby/Math/Math.h>

#include <box2d/box2d.h>
// #include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Tabby {

// ----- TransformComponent ---------------------
void TransformComponent::ApplyTransform(const Matrix4& transform)
{

    // using namespace glm;
    using T = float;

    Matrix4 LocalMatrix(transform);

    // Normalize the matrix.
    if (glm::epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), glm::epsilon<T>()))
        return;

    // First, isolate perspective.  This is the messiest.
    if (glm::epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), glm::epsilon<T>()) || glm::epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), glm::epsilon<T>()) || glm::epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), glm::epsilon<T>())) {
        // Clear the perspective partition
        LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
        LocalMatrix[3][3] = static_cast<T>(1);
    }

    // Next take care of translation (easy).
    Translation = Vector3(LocalMatrix[3]);
    LocalMatrix[3] = Vector4(0, 0, 0, LocalMatrix[3].w);

    Vector3 Row[3], Pdum3;

    // Now get scale and shear.
    for (glm::length_t i = 0; i < 3; ++i)
        for (glm::length_t j = 0; j < 3; ++j)
            Row[i][j] = LocalMatrix[i][j];

    // Compute X scale factor and normalize first row.
    Scale.x = length(Row[0]);
    Row[0] = glm::detail::scale(Row[0], static_cast<T>(1));
    Scale.y = length(Row[1]);
    Row[1] = glm::detail::scale(Row[1], static_cast<T>(1));
    Scale.z = length(Row[2]);
    Row[2] = glm::detail::scale(Row[2], static_cast<T>(1));

    // At this point, the matrix (in rows[]) is orthonormal.
    // Check for a coordinate system flip.  If the determinant
    // is -1, then negate the matrix and the scaling factors.
#if 0
        		Pdum3 = cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
        		if (dot(Row[0], Pdum3) < 0)
        		{
        			for (length_t i = 0; i < 3; i++)
        			{
        				scale[i] *= static_cast<T>(-1);
        				Row[i] *= static_cast<T>(-1);
        			}
        		}
#endif

    Rotation.y = asin(-Row[0][2]);
    if (cos(Rotation.y) != 0) {
        Rotation.x = atan2(Row[1][2], Row[2][2]);
        Rotation.z = atan2(Row[0][1], Row[0][0]);
    } else {
        Rotation.x = atan2(-Row[2][0], Row[1][1]);
        Rotation.z = 0;
    }

    Rotation.x *= Math::RAD2DEG;
    Rotation.y *= Math::RAD2DEG;
    Rotation.z *= Math::RAD2DEG;
}

void TransformComponent::ApplyTransformToLocal(const Matrix4& transform)
{

    // using namespace glm;
    using T = float;

    Matrix4 LocalMatrix(transform);

    // Normalize the matrix.
    if (glm::epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), glm::epsilon<T>()))
        return;

    // First, isolate perspective.  This is the messiest.
    if (glm::epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), glm::epsilon<T>()) || glm::epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), glm::epsilon<T>()) || glm::epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), glm::epsilon<T>())) {
        // Clear the perspective partition
        LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
        LocalMatrix[3][3] = static_cast<T>(1);
    }

    // Next take care of translation (easy).
    LocalTranslation = Vector3(LocalMatrix[3]);
    LocalMatrix[3] = Vector4(0, 0, 0, LocalMatrix[3].w);

    Vector3 Row[3], Pdum3;

    // Now get scale and shear.
    for (glm::length_t i = 0; i < 3; ++i)
        for (glm::length_t j = 0; j < 3; ++j)
            Row[i][j] = LocalMatrix[i][j];

    // Compute X scale factor and normalize first row.
    LocalScale.x = length(Row[0]);
    Row[0] = glm::detail::scale(Row[0], static_cast<T>(1));
    LocalScale.y = length(Row[1]);
    Row[1] = glm::detail::scale(Row[1], static_cast<T>(1));
    LocalScale.z = length(Row[2]);
    Row[2] = glm::detail::scale(Row[2], static_cast<T>(1));

    // At this point, the matrix (in rows[]) is orthonormal.
    // Check for a coordinate system flip.  If the determinant
    // is -1, then negate the matrix and the scaling factors.
#if 0
        		Pdum3 = cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
        		if (dot(Row[0], Pdum3) < 0)
        		{
        			for (length_t i = 0; i < 3; i++)
        			{
        				scale[i] *= static_cast<T>(-1);
        				Row[i] *= static_cast<T>(-1);
        			}
        		}
#endif

    LocalRotation.y = asin(-Row[0][2]);
    if (cos(Rotation.y) != 0) {
        LocalRotation.x = atan2(Row[1][2], Row[2][2]);
        LocalRotation.z = atan2(Row[0][1], Row[0][0]);
    } else {
        LocalRotation.x = atan2(-Row[2][0], Row[1][1]);
        LocalRotation.z = 0;
    }

    LocalRotation.x *= Math::RAD2DEG;
    LocalRotation.y *= Math::RAD2DEG;
    LocalRotation.z *= Math::RAD2DEG;
}

// --------------------------------------------------
//
//
//
// ----- Rigidbody2DComponent -----------------------

void Rigidbody2DComponent::SetBodyType(BodyType type)
{
    if (B2_IS_NON_NULL(RuntimeBodyId))
        b2Body_SetType(RuntimeBodyId, Utils::Rigidbody2DTypeToBox2DBody(type));
    Type = type;
}

void Rigidbody2DComponent::SetFixedRotation(bool enable)
{
    if (B2_IS_NON_NULL(RuntimeBodyId))
        b2Body_SetFixedRotation(RuntimeBodyId, enable);
    FixedRotation = enable;
}

void Rigidbody2DComponent::SetVelocity(Vector2 velocity)
{
    if (B2_IS_NON_NULL(RuntimeBodyId))
        b2Body_SetLinearVelocity(RuntimeBodyId, { velocity.x, velocity.y });
}

void Rigidbody2DComponent::SetAngularVelocity(float velocity)
{
    if (B2_IS_NON_NULL(RuntimeBodyId))
        b2Body_SetAngularVelocity(RuntimeBodyId, velocity);
}

Rigidbody2DComponent::BodyType Rigidbody2DComponent::GetType() const
{
    return Type;
}

Vector2 Rigidbody2DComponent::GetVelocity() const
{
    if (B2_IS_NON_NULL(RuntimeBodyId)) {
        b2Vec2 velocity = b2Body_GetLinearVelocity(RuntimeBodyId);
        return { velocity.x, velocity.y };
    } else
        return { 0.0f, 0.0f };
}

float Rigidbody2DComponent::GetAngularVelocity() const
{
    if (B2_IS_NON_NULL(RuntimeBodyId))
        return b2Body_GetAngularVelocity(RuntimeBodyId);
    else
        return 0.0f;
}

Vector2 Rigidbody2DComponent::GetPosition() const
{
    if (B2_IS_NON_NULL(RuntimeBodyId)) {
        b2Vec2 position = b2Body_GetPosition(RuntimeBodyId);
        return { position.x, position.y };
    } else
        return { 0.0f, 0.0f };
}

float Rigidbody2DComponent::GetAngle() const
{
    float angleRadians;

    if (B2_IS_NON_NULL(RuntimeBodyId))
        angleRadians = b2Rot_GetAngle(b2Body_GetRotation(RuntimeBodyId));

    float angleDegrees = angleRadians * Math::RAD2DEG;

    angleDegrees = std::fmod(angleDegrees, 360.0f);

    if (angleDegrees < 0.0f) {
        angleDegrees += 360.0f;
    }

    return angleDegrees;
}
// --------------------------------------------------
//
//
//
// ----- BoxCollider2DComponent -----------------------

void BoxCollider2DComponent::SetCollisionLayer(uint32_t layer)
{
    CollisionLayer = layer;
    if (B2_IS_NON_NULL(RuntimeShapeId)) {
        b2Filter filter = b2Shape_GetFilter(RuntimeShapeId);
        filter.categoryBits = layer;
        b2Shape_SetFilter(RuntimeShapeId, filter);
    }
}
uint32_t BoxCollider2DComponent::GetCollisionLayer() const
{
    return CollisionLayer;
}

void BoxCollider2DComponent::SetCollisionMask(uint32_t mask)
{
    CollisionMask = mask;
    if (B2_IS_NON_NULL(RuntimeShapeId)) {
        b2Filter filter = b2Shape_GetFilter(RuntimeShapeId);
        filter.maskBits = mask;
        b2Shape_SetFilter(RuntimeShapeId, filter);
    }
}
uint32_t BoxCollider2DComponent::GetCollisionMask() const
{
    return CollisionMask;
}

void BoxCollider2DComponent::SetCollisionLayerValue(int layerNumber, bool value)
{
    TB_CORE_VERIFY_TAGGED(layerNumber >= 1, "Collision layer number must be between 1 and 32 inclusive.");
    TB_CORE_VERIFY_TAGGED(layerNumber <= 32, "Collision layer number must be between 1 and 32 inclusive.");
    if (value) {
        CollisionLayer |= 1 << (layerNumber - 1);
    } else {
        CollisionLayer &= ~(1 << (layerNumber - 1));
    }

    if (B2_IS_NON_NULL(RuntimeShapeId)) {
        b2Filter filter = b2Shape_GetFilter(RuntimeShapeId);
        filter.categoryBits = CollisionLayer;
        b2Shape_SetFilter(RuntimeShapeId, filter);
    }
}
bool BoxCollider2DComponent::GetCollisionLayerValue(int layerNumber) const
{
    TB_CORE_VERIFY_TAGGED(layerNumber >= 1, "Collision layer number must be between 1 and 32 inclusive.");
    TB_CORE_VERIFY_TAGGED(layerNumber <= 32, "Collision layer number must be between 1 and 32 inclusive.");
    return CollisionLayer & (1 << (layerNumber - 1));
}

void BoxCollider2DComponent::SetCollisionMaskValue(int layerNumber, bool value)
{
    TB_CORE_VERIFY_TAGGED(layerNumber >= 1, "Collision layer number must be between 1 and 32 inclusive.");
    TB_CORE_VERIFY_TAGGED(layerNumber <= 32, "Collision layer number must be between 1 and 32 inclusive.");
    if (value) {
        CollisionMask |= 1 << (layerNumber - 1);
    } else {
        CollisionMask &= ~(1 << (layerNumber - 1));
    }

    if (B2_IS_NON_NULL(RuntimeShapeId)) {
        b2Filter filter = b2Shape_GetFilter(RuntimeShapeId);
        filter.maskBits = CollisionMask;
        b2Shape_SetFilter(RuntimeShapeId, filter);
    }
}
bool BoxCollider2DComponent::GetCollisionMaskValue(int layerNumber) const
{
    TB_CORE_VERIFY_TAGGED(layerNumber >= 1, "Collision layer number must be between 1 and 32 inclusive.");
    TB_CORE_VERIFY_TAGGED(layerNumber <= 32, "Collision layer number must be between 1 and 32 inclusive.");
    return CollisionMask & (1 << (layerNumber - 1));
}

void BoxCollider2DComponent::RefreshShape()
{
    ShapeInfo2D shapeInfo {
        static_cast<ShapeUserData2D*>(b2Shape_GetUserData(RuntimeShapeId))->shapeEntity,
        ColliderType2D::Circle
    };

    Physisc2D::EnqueueShapeUpdate(shapeInfo);
}

// --------------------------------------------------
//
//
//
// ----- CircleCollider2DComponent -----------------------

void CircleCollider2DComponent::SetCollisionLayer(uint32_t layer)
{
    CollisionLayer = layer;
    if (B2_IS_NON_NULL(RuntimeShapeId)) {
        b2Filter filter = b2Shape_GetFilter(RuntimeShapeId);
        filter.categoryBits = layer;
        b2Shape_SetFilter(RuntimeShapeId, filter);
    }
}
uint32_t CircleCollider2DComponent::GetCollisionLayer() const
{
    return CollisionLayer;
}

void CircleCollider2DComponent::SetCollisionMask(uint32_t mask)
{
    CollisionMask = mask;
    if (B2_IS_NON_NULL(RuntimeShapeId)) {
        b2Filter filter = b2Shape_GetFilter(RuntimeShapeId);
        filter.maskBits = mask;
        b2Shape_SetFilter(RuntimeShapeId, filter);
    }
}
uint32_t CircleCollider2DComponent::GetCollisionMask() const
{
    return CollisionMask;
}

void CircleCollider2DComponent::SetCollisionLayerValue(int layerNumber, bool value)
{
    TB_CORE_VERIFY_TAGGED(layerNumber >= 1, "Collision layer number must be between 1 and 32 inclusive.");
    TB_CORE_VERIFY_TAGGED(layerNumber <= 32, "Collision layer number must be between 1 and 32 inclusive.");
    if (value) {
        CollisionLayer |= 1 << (layerNumber - 1);
    } else {
        CollisionLayer &= ~(1 << (layerNumber - 1));
    }

    if (B2_IS_NON_NULL(RuntimeShapeId)) {
        b2Filter filter = b2Shape_GetFilter(RuntimeShapeId);
        filter.categoryBits = CollisionLayer;
        b2Shape_SetFilter(RuntimeShapeId, filter);
    }
}
bool CircleCollider2DComponent::GetCollisionLayerValue(int layerNumber) const
{
    TB_CORE_VERIFY_TAGGED(layerNumber >= 1, "Collision layer number must be between 1 and 32 inclusive.");
    TB_CORE_VERIFY_TAGGED(layerNumber <= 32, "Collision layer number must be between 1 and 32 inclusive.");
    return CollisionLayer & (1 << (layerNumber - 1));
}

void CircleCollider2DComponent::SetCollisionMaskValue(int layerNumber, bool value)
{
    TB_CORE_VERIFY_TAGGED(layerNumber >= 1, "Collision layer number must be between 1 and 32 inclusive.");
    TB_CORE_VERIFY_TAGGED(layerNumber <= 32, "Collision layer number must be between 1 and 32 inclusive.");
    if (value) {
        CollisionMask |= 1 << (layerNumber - 1);
    } else {
        CollisionMask &= ~(1 << (layerNumber - 1));
    }

    if (B2_IS_NON_NULL(RuntimeShapeId)) {
        b2Filter filter = b2Shape_GetFilter(RuntimeShapeId);
        filter.maskBits = CollisionMask;
        b2Shape_SetFilter(RuntimeShapeId, filter);
    }
}
bool CircleCollider2DComponent::GetCollisionMaskValue(int layerNumber) const
{
    TB_CORE_VERIFY_TAGGED(layerNumber >= 1, "Collision layer number must be between 1 and 32 inclusive.");
    TB_CORE_VERIFY_TAGGED(layerNumber <= 32, "Collision layer number must be between 1 and 32 inclusive.");
    return CollisionMask & (1 << (layerNumber - 1));
}

void CircleCollider2DComponent::RefreshShape()
{
    ShapeInfo2D shapeInfo {
        static_cast<ShapeUserData2D*>(b2Shape_GetUserData(RuntimeShapeId))->shapeEntity,
        ColliderType2D::Circle
    };

    Physisc2D::EnqueueShapeUpdate(shapeInfo);
}

// --------------------------------------------------
//
//
//
// ----- CapsuleCollider2DComponent -----------------------

void CapsuleCollider2DComponent::SetCollisionLayer(uint32_t layer)
{
    CollisionLayer = layer;
    if (B2_IS_NON_NULL(RuntimeShapeId)) {
        b2Filter filter = b2Shape_GetFilter(RuntimeShapeId);
        filter.categoryBits = layer;
        b2Shape_SetFilter(RuntimeShapeId, filter);
    }
}
uint32_t CapsuleCollider2DComponent::GetCollisionLayer() const
{
    return CollisionLayer;
}

void CapsuleCollider2DComponent::SetCollisionMask(uint32_t mask)
{
    CollisionMask = mask;
    if (B2_IS_NON_NULL(RuntimeShapeId)) {
        b2Filter filter = b2Shape_GetFilter(RuntimeShapeId);
        filter.maskBits = mask;
        b2Shape_SetFilter(RuntimeShapeId, filter);
    }
}
uint32_t CapsuleCollider2DComponent::GetCollisionMask() const
{
    return CollisionMask;
}

void CapsuleCollider2DComponent::SetCollisionLayerValue(int layerNumber, bool value)
{
    TB_CORE_VERIFY_TAGGED(layerNumber >= 1, "Collision layer number must be between 1 and 32 inclusive.");
    TB_CORE_VERIFY_TAGGED(layerNumber <= 32, "Collision layer number must be between 1 and 32 inclusive.");
    if (value) {
        CollisionLayer |= 1 << (layerNumber - 1);
    } else {
        CollisionLayer &= ~(1 << (layerNumber - 1));
    }

    if (B2_IS_NON_NULL(RuntimeShapeId)) {
        b2Filter filter = b2Shape_GetFilter(RuntimeShapeId);
        filter.categoryBits = CollisionLayer;
        b2Shape_SetFilter(RuntimeShapeId, filter);
    }
}
bool CapsuleCollider2DComponent::GetCollisionLayerValue(int layerNumber) const
{
    TB_CORE_VERIFY_TAGGED(layerNumber >= 1, "Collision layer number must be between 1 and 32 inclusive.");
    TB_CORE_VERIFY_TAGGED(layerNumber <= 32, "Collision layer number must be between 1 and 32 inclusive.");
    return CollisionLayer & (1 << (layerNumber - 1));
}

void CapsuleCollider2DComponent::SetCollisionMaskValue(int layerNumber, bool value)
{
    TB_CORE_VERIFY_TAGGED(layerNumber >= 1, "Collision layer number must be between 1 and 32 inclusive.");
    TB_CORE_VERIFY_TAGGED(layerNumber <= 32, "Collision layer number must be between 1 and 32 inclusive.");
    if (value) {
        CollisionMask |= 1 << (layerNumber - 1);
    } else {
        CollisionMask &= ~(1 << (layerNumber - 1));
    }

    if (B2_IS_NON_NULL(RuntimeShapeId)) {
        b2Filter filter = b2Shape_GetFilter(RuntimeShapeId);
        filter.maskBits = CollisionMask;
        b2Shape_SetFilter(RuntimeShapeId, filter);
    }
}
bool CapsuleCollider2DComponent::GetCollisionMaskValue(int layerNumber) const
{
    TB_CORE_VERIFY_TAGGED(layerNumber >= 1, "Collision layer number must be between 1 and 32 inclusive.");
    TB_CORE_VERIFY_TAGGED(layerNumber <= 32, "Collision layer number must be between 1 and 32 inclusive.");
    return CollisionMask & (1 << (layerNumber - 1));
}
void CapsuleCollider2DComponent::RefreshShape()
{
    ShapeInfo2D shapeInfo {
        static_cast<ShapeUserData2D*>(b2Shape_GetUserData(RuntimeShapeId))->shapeEntity,
        ColliderType2D::Circle
    };

    Physisc2D::EnqueueShapeUpdate(shapeInfo);
}

// --------------------------------------------------
//
//
//
// ----- SegmentCollider2DComponent -----------------------

void SegmentCollider2DComponent::SetCollisionLayer(uint32_t layer)
{
    CollisionLayer = layer;
    if (B2_IS_NON_NULL(RuntimeShapeId)) {
        b2Filter filter = b2Shape_GetFilter(RuntimeShapeId);
        filter.categoryBits = layer;
        b2Shape_SetFilter(RuntimeShapeId, filter);
    }
}
uint32_t SegmentCollider2DComponent::GetCollisionLayer() const
{
    return CollisionLayer;
}

void SegmentCollider2DComponent::SetCollisionMask(uint32_t mask)
{
    CollisionMask = mask;
    if (B2_IS_NON_NULL(RuntimeShapeId)) {
        b2Filter filter = b2Shape_GetFilter(RuntimeShapeId);
        filter.maskBits = mask;
        b2Shape_SetFilter(RuntimeShapeId, filter);
    }
}
uint32_t SegmentCollider2DComponent::GetCollisionMask() const
{
    return CollisionMask;
}

void SegmentCollider2DComponent::SetCollisionLayerValue(int layerNumber, bool value)
{
    TB_CORE_VERIFY_TAGGED(layerNumber >= 1, "Collision layer number must be between 1 and 32 inclusive.");
    TB_CORE_VERIFY_TAGGED(layerNumber <= 32, "Collision layer number must be between 1 and 32 inclusive.");
    if (value) {
        CollisionLayer |= 1 << (layerNumber - 1);
    } else {
        CollisionLayer &= ~(1 << (layerNumber - 1));
    }

    if (B2_IS_NON_NULL(RuntimeShapeId)) {
        b2Filter filter = b2Shape_GetFilter(RuntimeShapeId);
        filter.categoryBits = CollisionLayer;
        b2Shape_SetFilter(RuntimeShapeId, filter);
    }
}
bool SegmentCollider2DComponent::GetCollisionLayerValue(int layerNumber) const
{
    TB_CORE_VERIFY_TAGGED(layerNumber >= 1, "Collision layer number must be between 1 and 32 inclusive.");
    TB_CORE_VERIFY_TAGGED(layerNumber <= 32, "Collision layer number must be between 1 and 32 inclusive.");
    return CollisionLayer & (1 << (layerNumber - 1));
}

void SegmentCollider2DComponent::SetCollisionMaskValue(int layerNumber, bool value)
{
    TB_CORE_VERIFY_TAGGED(layerNumber >= 1, "Collision layer number must be between 1 and 32 inclusive.");
    TB_CORE_VERIFY_TAGGED(layerNumber <= 32, "Collision layer number must be between 1 and 32 inclusive.");
    if (value) {
        CollisionMask |= 1 << (layerNumber - 1);
    } else {
        CollisionMask &= ~(1 << (layerNumber - 1));
    }

    if (B2_IS_NON_NULL(RuntimeShapeId)) {
        b2Filter filter = b2Shape_GetFilter(RuntimeShapeId);
        filter.maskBits = CollisionMask;
        b2Shape_SetFilter(RuntimeShapeId, filter);
    }
}
bool SegmentCollider2DComponent::GetCollisionMaskValue(int layerNumber) const
{
    TB_CORE_VERIFY_TAGGED(layerNumber >= 1, "Collision layer number must be between 1 and 32 inclusive.");
    TB_CORE_VERIFY_TAGGED(layerNumber <= 32, "Collision layer number must be between 1 and 32 inclusive.");
    return CollisionMask & (1 << (layerNumber - 1));
}

void SegmentCollider2DComponent::RefreshShape()
{
    ShapeInfo2D shapeInfo {
        static_cast<ShapeUserData2D*>(b2Shape_GetUserData(RuntimeShapeId))->shapeEntity,
        ColliderType2D::Segment
    };

    Physisc2D::EnqueueShapeUpdate(shapeInfo);
}

}
