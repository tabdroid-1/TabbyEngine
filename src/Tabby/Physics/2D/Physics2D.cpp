#include <Tabby/Physics/2D/Physics2D.h>
#include <Tabby/Physics/2D/Physics2DTypes.h>
#include <Tabby/Physics/2D/Physics2DUtil.h>
#include <Tabby/Core/Time/Time.h>
#include <Tabby/Math/Math.h>

#include <box2d/box2d.h>
#include "box2d/math_functions.h"
#include <entt.hpp>

namespace Tabby {

static bool Physics2DPreSolve(b2ShapeId shapeIdA, b2ShapeId shapeIdB, b2Manifold* manifold, void* context);
static bool Physics2DPostSolve(b2ShapeId shapeIdA, b2ShapeId shapeIdB, b2Manifold* manifold, void* context);

Physisc2D* Physisc2D::s_Instance = nullptr;

Physisc2D::Physisc2D()
{
    if (!s_Instance)
        s_Instance = this;

    m_PhysicsWorld = b2_nullWorldId;
}

Physisc2D::~Physisc2D()
{
    b2DestroyWorld(m_PhysicsWorld);
    m_PhysicsWorld = b2_nullWorldId;
}

void Physisc2D::Init()
{
    if (!s_Instance)
        s_Instance = new Physisc2D();

    if (B2_IS_NULL(s_Instance->m_PhysicsWorld)) {

        b2WorldDef worldDef = b2DefaultWorldDef();
        worldDef.gravity = { 0.0f, -9.81f };
        s_Instance->m_PhysicsWorld = b2CreateWorld(&worldDef);
        b2World_SetPreSolveCallback(s_Instance->m_PhysicsWorld, Physics2DPreSolve, nullptr);
    }
}

void Physisc2D::Init(const Vector2& gravity)
{
    if (!s_Instance)
        s_Instance = new Physisc2D();

    if (B2_IS_NULL(s_Instance->m_PhysicsWorld)) {

        b2WorldDef worldDef = b2DefaultWorldDef();
        worldDef.gravity = { gravity.x, gravity.y };
        s_Instance->m_PhysicsWorld = b2CreateWorld(&worldDef);
    }
}

void Physisc2D::UpdateWorld()
{
    ProcessBodyInitQueue();
    ProcessShapeInitQueue();
    ProcessShapeUpdateQueue();

    TB_CORE_ASSERT_TAGGED(s_Instance, "Physisc2D: Instance have to be initialized first!");
    b2World_Step(s_Instance->m_PhysicsWorld, Time::GetDeltaTime(), s_Instance->m_SubstepCount);

    ProcessEvents();
}

uint8_t Physisc2D::GetSubstepCount()
{
    return s_Instance->m_SubstepCount;
}

void Physisc2D::SetSubstepCount(uint8_t substepCount)
{
    if (substepCount < 1) {
        TB_CORE_ERROR("Physics2D: SubstepCount can not be lower than 1!");
        return;
    }
    s_Instance->m_SubstepCount = substepCount;
}

Vector2 Physisc2D::GetGravity()
{
    Vector2 gravity;
    if (B2_IS_NULL(s_Instance->m_PhysicsWorld)) {

        const auto& b2grav = b2World_GetGravity(s_Instance->m_PhysicsWorld);
        gravity = { b2grav.x, b2grav.y };
    }
    return gravity;
}
void Physisc2D::SetGravity(Vector2 gravity)
{
    if (B2_IS_NULL(s_Instance->m_PhysicsWorld))
        b2World_SetGravity(s_Instance->m_PhysicsWorld, { gravity.x, gravity.y });
    else
        TB_CORE_WARN("Physisc2D: Physics world is null. Gravity is not set!");
}

b2WorldId Physisc2D::GetPhysicsWorld()
{
    if (s_Instance && B2_IS_NON_NULL(s_Instance->m_PhysicsWorld))
        return s_Instance->m_PhysicsWorld;
    else {
        TB_CORE_ERROR("Physics2D: Physics world is not initialized!");
        return b2_nullWorldId;
    }
}

RaycastHit2D Physisc2D::RayCast(const Vector2& origin, const Vector2& direction, float distance, RaycastFilter2D raycastFilter, int minDepth, int maxDepth)
{
    RaycastHit2D tempRayCastHit;
    tempRayCastHit.RaycastFilter = raycastFilter;
    tempRayCastHit.origin = origin;

    b2CastResultFcn* fcn = Physics2DRaycastCallback;
    b2Vec2 box2DOrigin = { origin.x, origin.y };
    b2Vec2 box2DDirection = { direction.x, direction.y };
    b2Vec2 box2DDestination = { direction.x * distance, direction.y * distance };
    b2World_CastRay(s_Instance->m_PhysicsWorld, box2DOrigin, box2DDestination, { raycastFilter.GetCollisionLayer(), raycastFilter.GetCollisionMask() }, fcn, &tempRayCastHit);

    return tempRayCastHit;
}

RaycastHit2D Physisc2D::RayCast(const Vector2& origin, const Vector2& direction, float distance, RaycastFilter2D raycastFilter)
{
    return RayCast(origin, direction, distance, raycastFilter, 0, std::numeric_limits<int>::max());
}

RaycastHit2D Physisc2D::RayCast(const Vector2& origin, const Vector2& direction, float distance)
{
    return RayCast(origin, direction, distance, RaycastFilter2D(), 0, std::numeric_limits<int>::max());
}

RaycastHit2D Physisc2D::RayCast(const Vector2& origin, const Vector2& direction)
{
    return RayCast(origin, direction, 2000.0f, RaycastFilter2D(), 0, std::numeric_limits<int>::max());
}

RaycastHit2D Physisc2D::BoxCast(const Vector2& boxSize, const Vector2& origin, const Vector2& direction, float distance, RaycastFilter2D raycastFilter, int minDepth, int maxDepth)
{
    RaycastHit2D tempRayCastHit;

    tempRayCastHit.origin = origin;

    b2Polygon box = b2MakeBox(boxSize.x, boxSize.y);

    b2CastResultFcn* fcn = Physics2DRaycastCallback;
    b2Transform box2DOrigin = { { origin.x, origin.y }, b2MakeRot(0.0f) };
    b2Vec2 box2DDirection = { direction.x, direction.y };
    b2Vec2 box2DDestination = { direction.x * distance, direction.y * distance };

    b2World_CastPolygon(s_Instance->m_PhysicsWorld, &box, box2DOrigin, box2DDestination, { raycastFilter.GetCollisionLayer(), raycastFilter.GetCollisionMask() }, fcn, &tempRayCastHit);

    return tempRayCastHit;
}

RaycastHit2D Physisc2D::BoxCast(const Vector2& boxSize, const Vector2& origin, const Vector2& direction, float distance, RaycastFilter2D raycastFilter)
{
    return BoxCast(boxSize, origin, direction, distance, RaycastFilter2D(), 0, std::numeric_limits<int>::max());
}

RaycastHit2D Physisc2D::BoxCast(const Vector2& boxSize, const Vector2& origin, const Vector2& direction, float distance)
{
    return BoxCast(boxSize, origin, direction, distance, RaycastFilter2D(), 0, std::numeric_limits<int>::max());
}

RaycastHit2D Physisc2D::BoxCast(const Vector2& boxSize, const Vector2& origin, const Vector2& direction)
{
    return BoxCast(boxSize, origin, direction, 2000.0f, RaycastFilter2D(), 0, std::numeric_limits<int>::max());
}

RaycastHit2D Physisc2D::CapsuleCast(const Vector2& point1, const Vector2& point2, float radius, const Vector2& origin, const Vector2& direction, float distance, RaycastFilter2D raycastFilter, int minDepth, int maxDepth)
{
    RaycastHit2D tempRayCastHit;

    tempRayCastHit.origin = origin;

    b2Capsule capsule = { { point1.x, point1.y }, { point2.x, point2.y }, radius };

    b2CastResultFcn* fcn = Physics2DRaycastCallback;
    b2Transform box2DOrigin = { { origin.x, origin.y }, b2MakeRot(0.0f) };
    b2Vec2 box2DDirection = { direction.x, direction.y };
    b2Vec2 box2DDestination = { direction.x * distance, direction.y * distance };

    b2World_CastCapsule(s_Instance->m_PhysicsWorld, &capsule, box2DOrigin, box2DDestination, { raycastFilter.GetCollisionLayer(), raycastFilter.GetCollisionMask() }, fcn, &tempRayCastHit);

    return tempRayCastHit;
}

RaycastHit2D Physisc2D::CapsuleCast(const Vector2& point1, const Vector2& point2, float radius, const Vector2& origin, const Vector2& direction, float distance, RaycastFilter2D raycastFilter)
{
    return CapsuleCast(point1, point2, radius, origin, direction, distance, RaycastFilter2D(), 0, std::numeric_limits<int>::max());
}

RaycastHit2D Physisc2D::CapsuleCast(const Vector2& point1, const Vector2& point2, float radius, const Vector2& origin, const Vector2& direction, float distance)
{
    return CapsuleCast(point1, point2, radius, origin, direction, distance, RaycastFilter2D(), 0, std::numeric_limits<int>::max());
}

RaycastHit2D Physisc2D::CapsuleCast(const Vector2& point1, const Vector2& point2, float radius, const Vector2& origin, const Vector2& direction)
{
    return CapsuleCast(point1, point2, radius, origin, direction, 2000.0f, RaycastFilter2D(), 0, std::numeric_limits<int>::max());
}

RaycastHit2D Physisc2D::CircleCast(float radius, const Vector2& origin, const Vector2& direction, float distance, RaycastFilter2D raycastFilter, int minDepth, int maxDepth)
{
    RaycastHit2D tempRayCastHit;

    tempRayCastHit.origin = origin;

    b2Circle circle = { { 0.0f, 0.0f }, radius };

    b2CastResultFcn* fcn = Physics2DRaycastCallback;
    b2Transform box2DOrigin = { { origin.x, origin.y }, b2MakeRot(0.0f) };
    b2Vec2 box2DDirection = { direction.x, direction.y };
    b2Vec2 box2DDestination = { direction.x * distance, direction.y * distance };

    b2World_CastCircle(s_Instance->m_PhysicsWorld, &circle, box2DOrigin, box2DDestination, { raycastFilter.GetCollisionLayer(), raycastFilter.GetCollisionMask() }, fcn, &tempRayCastHit);

    return tempRayCastHit;
}

RaycastHit2D Physisc2D::CircleCast(float radius, const Vector2& origin, const Vector2& direction, float distance, RaycastFilter2D raycastFilter)
{
    return CircleCast(radius, origin, direction, distance, RaycastFilter2D(), 0, std::numeric_limits<int>::max());
}

RaycastHit2D Physisc2D::CircleCast(float radius, const Vector2& origin, const Vector2& direction, float distance)
{
    return CircleCast(radius, origin, direction, distance, RaycastFilter2D(), 0, std::numeric_limits<int>::max());
}

RaycastHit2D Physisc2D::CircleCast(float radius, const Vector2& origin, const Vector2& direction)
{
    return CircleCast(radius, origin, direction, 2000.0f, RaycastFilter2D(), 0, std::numeric_limits<int>::max());
}

void Physisc2D::EnqueueBodyInit(BodyInfo2D bodyInfo)
{
    s_Instance->bodyInitQueue.push(bodyInfo);
}

void Physisc2D::EnqueueShapeInit(ShapeInfo2D shapeInfo)
{
    s_Instance->shapeInitQueue.push(shapeInfo);
}

void Physisc2D::EnqueueShapeUpdate(ShapeInfo2D shapeInfo)
{
    s_Instance->shapeUpdateQueue.push(shapeInfo);
}

void Physisc2D::ProcessBodyInitQueue()
{
    while (!s_Instance->bodyInitQueue.empty()) {

        s_Instance->queueEmpty = false;

        BodyInfo2D bodyInfo = s_Instance->bodyInitQueue.front();
        auto& transform = bodyInfo.entity.GetComponent<TransformComponent>();
        auto& rb2d = bodyInfo.entity.GetComponent<Rigidbody2DComponent>();

        // --------- Create Body def ---------
        BodyUserData2D* bodyUserData = new BodyUserData2D { bodyInfo.entity };

        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = Utils::Rigidbody2DTypeToBox2DBody(rb2d.Type);
        bodyDef.position = { transform.Translation.x, transform.Translation.y };
        bodyDef.rotation = b2MakeRot(transform.Rotation.z * Math::DEG2RAD);
        bodyDef.userData = static_cast<void*>(bodyUserData);

        // --------- Create Body ---------
        rb2d.RuntimeBodyId = b2CreateBody(s_Instance->m_PhysicsWorld, &bodyDef);
        b2Body_SetFixedRotation(rb2d.RuntimeBodyId, rb2d.FixedRotation);

        s_Instance->bodyInitQueue.pop();
    }

    if (s_Instance->bodyInitQueue.empty() && s_Instance->shapeInitQueue.empty() && s_Instance->shapeUpdateQueue.empty())
        s_Instance->queueEmpty = true;
}

void Physisc2D::ProcessShapeInitQueue()
{
    while (!s_Instance->shapeInitQueue.empty()) {
        s_Instance->queueEmpty = false;

        ShapeInfo2D shapeInfo = s_Instance->shapeInitQueue.front();
        auto& transform = shapeInfo.entity.GetComponent<TransformComponent>();

        // Check if entity it self has Rigidbody2DComponent. If there is, collider will be added to Rigidbody2DComponent of same entity.
        // If there is not, function will check its parent if it has Rigidbody2DComponent. And this will go on untill it finds an entity Rigidbody2DComponent;
        // This is is because an entity can have one component of same type. User can create More colliders by creating child entities.
        entt::entity rb2dEntity { entt::null };
        if (shapeInfo.entity.HasComponent<Rigidbody2DComponent>()) {
            rb2dEntity = shapeInfo.entity;
        } else {

            auto FindParentRigidbody = [](auto&& self, entt::entity parentEntity) -> entt::entity {
                auto& hierarchyNode = Entity(parentEntity).GetComponent<HierarchyNodeComponent>();

                if (Entity(hierarchyNode.Parent.second).HasComponent<Rigidbody2DComponent>()) {
                    return hierarchyNode.Parent.second;
                } else {
                    if (hierarchyNode.Parent.first.Valid())
                        return self(self, hierarchyNode.Parent.second);
                    else
                        return entt::null;
                }
            };

            rb2dEntity = FindParentRigidbody(FindParentRigidbody, shapeInfo.entity);
        }

        if (rb2dEntity == entt::null) {
            s_Instance->shapeInitQueue.pop();
            continue;
        }

        if (shapeInfo.colliderType == ColliderType2D::Box) {
            auto& bc2d = shapeInfo.entity.GetComponent<BoxCollider2DComponent>();
            ShapeUserData2D* userData = new ShapeUserData2D { shapeInfo.entity, rb2dEntity, ColliderType2D::Box };

            // --------- Create box collider def ---------
            b2Polygon box = b2MakeOffsetBox(bc2d.Size.x * transform.Scale.x, bc2d.Size.y * transform.Scale.y, { transform.LocalTranslation.x + bc2d.Offset.x, transform.LocalTranslation.y + bc2d.Offset.y }, bc2d.Angle);

            b2ShapeDef shapeDef = b2DefaultShapeDef();
            shapeDef.density = bc2d.Density;
            shapeDef.friction = bc2d.Friction;
            shapeDef.restitution = bc2d.Restitution;
            shapeDef.isSensor = bc2d.IsSensor;
            shapeDef.enableSensorEvents = bc2d.EnableSensorEvents;
            shapeDef.enableContactEvents = bc2d.EnableContactEvents;
            shapeDef.enablePreSolveEvents = bc2d.EnablePreSolveEvents;
            shapeDef.filter.categoryBits = bc2d.CollisionLayer;
            shapeDef.filter.maskBits = bc2d.CollisionMask;
            shapeDef.userData = static_cast<void*>(userData);

            // --------- Create box collider in body ---------
            bc2d.RuntimeShapeId = b2CreatePolygonShape(Entity(rb2dEntity).GetComponent<Rigidbody2DComponent>().RuntimeBodyId, &shapeDef, &box);
            bc2d.QueuedForInitialization = false;

        } else if (shapeInfo.colliderType == ColliderType2D::Circle) {
            auto& cc2d = shapeInfo.entity.GetComponent<CircleCollider2DComponent>();
            ShapeUserData2D* userData = new ShapeUserData2D { shapeInfo.entity, rb2dEntity, ColliderType2D::Circle };

            // --------- Create circle collider def ---------
            b2Circle circleShape = { { transform.LocalTranslation.x + cc2d.Offset.x, transform.LocalTranslation.y + cc2d.Offset.y }, cc2d.Radius };

            b2ShapeDef shapeDef = b2DefaultShapeDef();
            shapeDef.density = cc2d.Density;
            shapeDef.friction = cc2d.Friction;
            shapeDef.restitution = cc2d.Restitution;
            shapeDef.isSensor = cc2d.IsSensor;
            shapeDef.enableSensorEvents = cc2d.EnableSensorEvents;
            shapeDef.enableContactEvents = cc2d.EnableContactEvents;
            shapeDef.enablePreSolveEvents = cc2d.EnablePreSolveEvents;
            shapeDef.filter.categoryBits = cc2d.CollisionLayer;
            shapeDef.filter.maskBits = cc2d.CollisionMask;
            shapeDef.userData = static_cast<void*>(userData);

            // --------- Create circle collider in body ---------
            cc2d.RuntimeShapeId = b2CreateCircleShape(Entity(rb2dEntity).GetComponent<Rigidbody2DComponent>().RuntimeBodyId, &shapeDef, &circleShape);
            cc2d.QueuedForInitialization = false;
        } else if (shapeInfo.colliderType == ColliderType2D::Capsule) {
            auto& cc2d = shapeInfo.entity.GetComponent<CapsuleCollider2DComponent>();
            ShapeUserData2D* userData = new ShapeUserData2D { shapeInfo.entity, rb2dEntity, ColliderType2D::Capsule };

            // --------- Create circle collider def ---------

            b2Vec2 center1 = { cc2d.center1.x + transform.LocalTranslation.x, cc2d.center1.y + transform.LocalTranslation.y };
            b2Vec2 center2 = { cc2d.center2.x + transform.LocalTranslation.x, cc2d.center2.y + transform.LocalTranslation.y };
            b2Capsule capsuleShape = { center1, center2, cc2d.Radius };

            b2ShapeDef shapeDef = b2DefaultShapeDef();
            shapeDef.density = cc2d.Density;
            shapeDef.friction = cc2d.Friction;
            shapeDef.restitution = cc2d.Restitution;
            shapeDef.isSensor = cc2d.IsSensor;
            shapeDef.enableSensorEvents = cc2d.EnableSensorEvents;
            shapeDef.enableContactEvents = cc2d.EnableContactEvents;
            shapeDef.enablePreSolveEvents = cc2d.EnablePreSolveEvents;
            shapeDef.filter.categoryBits = cc2d.CollisionLayer;
            shapeDef.filter.maskBits = cc2d.CollisionMask;
            shapeDef.userData = static_cast<void*>(userData);

            // --------- Create circle collider in body ---------
            cc2d.RuntimeShapeId = b2CreateCapsuleShape(Entity(rb2dEntity).GetComponent<Rigidbody2DComponent>().RuntimeBodyId, &shapeDef, &capsuleShape);
            cc2d.QueuedForInitialization = false;
        } else if (shapeInfo.colliderType == ColliderType2D::Segment) {
            auto& sc2d = shapeInfo.entity.GetComponent<SegmentCollider2DComponent>();
            ShapeUserData2D* userData = new ShapeUserData2D { shapeInfo.entity, rb2dEntity, ColliderType2D::Segment };

            // --------- Create circle collider def ---------

            b2Vec2 point1 = { sc2d.point1.x + transform.LocalTranslation.x, sc2d.point1.y + transform.LocalTranslation.y };
            b2Vec2 point2 = { sc2d.point2.x + transform.LocalTranslation.x, sc2d.point2.y + transform.LocalTranslation.y };
            b2Segment segmentShape = { point1, point2 };

            b2ShapeDef shapeDef = b2DefaultShapeDef();
            shapeDef.density = sc2d.Density;
            shapeDef.friction = sc2d.Friction;
            shapeDef.restitution = sc2d.Restitution;
            shapeDef.isSensor = sc2d.IsSensor;
            shapeDef.enableSensorEvents = sc2d.EnableSensorEvents;
            shapeDef.enableContactEvents = sc2d.EnableContactEvents;
            shapeDef.enablePreSolveEvents = sc2d.EnablePreSolveEvents;
            shapeDef.filter.categoryBits = sc2d.CollisionLayer;
            shapeDef.filter.maskBits = sc2d.CollisionMask;
            shapeDef.userData = static_cast<void*>(userData);

            // --------- Create circle collider in body ---------
            sc2d.RuntimeShapeId = b2CreateSegmentShape(Entity(rb2dEntity).GetComponent<Rigidbody2DComponent>().RuntimeBodyId, &shapeDef, &segmentShape);
            sc2d.QueuedForInitialization = false;
        }

        s_Instance->shapeInitQueue.pop();
    }

    if (s_Instance->bodyInitQueue.empty() && s_Instance->shapeInitQueue.empty() && s_Instance->shapeUpdateQueue.empty())
        s_Instance->queueEmpty = true;
}

void Physisc2D::ProcessShapeUpdateQueue()
{
    while (!s_Instance->shapeUpdateQueue.empty()) {
        s_Instance->queueEmpty = false;

        ShapeInfo2D shapeInfo = s_Instance->shapeUpdateQueue.front();
        auto& transform = shapeInfo.entity.GetComponent<TransformComponent>();

        if (shapeInfo.colliderType == ColliderType2D::Box) {
            auto& bc2d = shapeInfo.entity.GetComponent<BoxCollider2DComponent>();

            // --------- Create box collider def ---------
            b2Polygon box = b2MakeOffsetBox(bc2d.Size.x * transform.Scale.x, bc2d.Size.y * transform.Scale.y, { transform.LocalTranslation.x + bc2d.Offset.x, transform.LocalTranslation.y + bc2d.Offset.y }, bc2d.Angle);

            b2Shape_SetPolygon(bc2d.RuntimeShapeId, &box);

            b2Filter filter = b2Shape_GetFilter(bc2d.RuntimeShapeId);
            filter.categoryBits = bc2d.CollisionLayer;
            filter.maskBits = bc2d.CollisionMask;
            b2Shape_SetFilter(bc2d.RuntimeShapeId, filter);

            bc2d.QueuedForInitialization = false;

        } else if (shapeInfo.colliderType == ColliderType2D::Circle) {
            auto& cc2d = shapeInfo.entity.GetComponent<CircleCollider2DComponent>();
            b2Circle circleShape = { { transform.LocalTranslation.x + cc2d.Offset.x, transform.LocalTranslation.y + cc2d.Offset.y }, cc2d.Radius };

            b2Shape_SetCircle(cc2d.RuntimeShapeId, &circleShape);

            b2Filter filter = b2Shape_GetFilter(cc2d.RuntimeShapeId);
            filter.categoryBits = cc2d.CollisionLayer;
            filter.maskBits = cc2d.CollisionMask;
            b2Shape_SetFilter(cc2d.RuntimeShapeId, filter);

            cc2d.QueuedForInitialization = false;

        } else if (shapeInfo.colliderType == ColliderType2D::Capsule) {
            auto& cc2d = shapeInfo.entity.GetComponent<CapsuleCollider2DComponent>();

            b2Vec2 center1 = { cc2d.center1.x + transform.LocalTranslation.x, cc2d.center1.y + transform.LocalTranslation.y };
            b2Vec2 center2 = { cc2d.center2.x + transform.LocalTranslation.x, cc2d.center2.y + transform.LocalTranslation.y };
            b2Capsule capsuleShape = { center1, center2, cc2d.Radius };

            // --------- Create circle collider in body ---------
            b2Shape_SetCapsule(cc2d.RuntimeShapeId, &capsuleShape);

            b2Filter filter = b2Shape_GetFilter(cc2d.RuntimeShapeId);
            filter.categoryBits = cc2d.CollisionLayer;
            filter.maskBits = cc2d.CollisionMask;
            b2Shape_SetFilter(cc2d.RuntimeShapeId, filter);

            cc2d.QueuedForInitialization = false;
        } else if (shapeInfo.colliderType == ColliderType2D::Segment) {
            auto& sc2d = shapeInfo.entity.GetComponent<SegmentCollider2DComponent>();

            b2Vec2 point1 = { sc2d.point1.x + transform.LocalTranslation.x, sc2d.point1.y + transform.LocalTranslation.y };
            b2Vec2 point2 = { sc2d.point2.x + transform.LocalTranslation.x, sc2d.point2.y + transform.LocalTranslation.y };
            b2Segment segmentShape = { point1, point2 };

            // --------- Create circle collider in body ---------
            b2Shape_SetSegment(sc2d.RuntimeShapeId, &segmentShape);

            b2Filter filter = b2Shape_GetFilter(sc2d.RuntimeShapeId);
            filter.categoryBits = sc2d.CollisionLayer;
            filter.maskBits = sc2d.CollisionMask;
            b2Shape_SetFilter(sc2d.RuntimeShapeId, filter);

            sc2d.QueuedForInitialization = false;
        }

        s_Instance->shapeUpdateQueue.pop();
    }

    if (s_Instance->bodyInitQueue.empty() && s_Instance->shapeInitQueue.empty() && s_Instance->shapeUpdateQueue.empty())
        s_Instance->queueEmpty = true;
}

float Physisc2D::Physics2DRaycastCallback(b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void* context)
{

    ShapeUserData2D* userData = static_cast<ShapeUserData2D*>(b2Shape_GetUserData(shapeId));

    static_cast<RaycastHit2D*>(context)->point = { point.x, point.y };
    static_cast<RaycastHit2D*>(context)->normal = { normal.x, normal.y };
    static_cast<RaycastHit2D*>(context)->fraction = fraction;

    static_cast<RaycastHit2D*>(context)->entity = userData->bodyEntity;
    static_cast<RaycastHit2D*>(context)->transform = &userData->bodyEntity.GetComponent<TransformComponent>();
    static_cast<RaycastHit2D*>(context)->rigidbody = &userData->bodyEntity.GetComponent<Rigidbody2DComponent>();
    static_cast<RaycastHit2D*>(context)->distance = glm::distance(static_cast<RaycastHit2D*>(context)->origin, { point.x, point.y });

    return fraction;
}

void Physisc2D::ProcessEvents()
{
    b2ContactEvents contactEvents = b2World_GetContactEvents(s_Instance->m_PhysicsWorld);
    for (int i = 0; i < contactEvents.beginCount; ++i) {
        b2ContactBeginTouchEvent event = contactEvents.beginEvents[i];

        // TODO: A way to recieve/process callbacks

        // --- Process Begin Touch Event -----------------
        b2BodyId bodyIdA = b2Shape_GetBody(event.shapeIdA);
        b2BodyId bodyIdB = b2Shape_GetBody(event.shapeIdB);

        BodyUserData2D* userDataA = static_cast<BodyUserData2D*>(b2Body_GetUserData(bodyIdA));
        BodyUserData2D* userDataB = static_cast<BodyUserData2D*>(b2Body_GetUserData(bodyIdB));

        if (userDataA->bodyEntity.HasComponent<Rigidbody2DComponent>()) {
            auto& rb = userDataA->bodyEntity.GetComponent<Rigidbody2DComponent>();

            ContactCallback callbackA;
            callbackA.entity = userDataB->bodyEntity;
            callbackA.transform = &userDataB->bodyEntity.GetComponent<TransformComponent>();
            callbackA.rigidbody = &userDataB->bodyEntity.GetComponent<Rigidbody2DComponent>();

            if (rb.OnCollisionEnterCallback) {
                rb.OnCollisionEnterCallback(callbackA);
            }
        }

        if (userDataB->bodyEntity.HasComponent<Rigidbody2DComponent>()) {
            auto& rb = userDataB->bodyEntity.GetComponent<Rigidbody2DComponent>();

            ContactCallback callbackB;
            callbackB.entity = userDataA->bodyEntity;
            callbackB.transform = &userDataA->bodyEntity.GetComponent<TransformComponent>();
            callbackB.rigidbody = &userDataA->bodyEntity.GetComponent<Rigidbody2DComponent>();

            if (rb.OnCollisionEnterCallback) {
                rb.OnCollisionEnterCallback(callbackB);
            }
        }
    }

    for (int i = 0; i < contactEvents.endCount; ++i) {
        b2ContactEndTouchEvent event = contactEvents.endEvents[i];

        // --- Process End Touch Event -----------------
        b2BodyId bodyIdA = b2Shape_GetBody(event.shapeIdA);
        b2BodyId bodyIdB = b2Shape_GetBody(event.shapeIdB);

        BodyUserData2D* userDataA = static_cast<BodyUserData2D*>(b2Body_GetUserData(bodyIdA));
        BodyUserData2D* userDataB = static_cast<BodyUserData2D*>(b2Body_GetUserData(bodyIdB));

        if (userDataA->bodyEntity.HasComponent<Rigidbody2DComponent>()) {
            auto& rb = userDataA->bodyEntity.GetComponent<Rigidbody2DComponent>();

            ContactCallback callbackA;
            callbackA.entity = userDataB->bodyEntity;
            callbackA.transform = &userDataB->bodyEntity.GetComponent<TransformComponent>();
            callbackA.rigidbody = &userDataB->bodyEntity.GetComponent<Rigidbody2DComponent>();

            if (rb.OnCollisionExitCallback) {
                rb.OnCollisionExitCallback(callbackA);
            }
        }

        if (userDataB->bodyEntity.HasComponent<Rigidbody2DComponent>()) {
            auto& rb = userDataB->bodyEntity.GetComponent<Rigidbody2DComponent>();

            ContactCallback callbackB;
            callbackB.entity = userDataA->bodyEntity;
            callbackB.transform = &userDataA->bodyEntity.GetComponent<TransformComponent>();
            callbackB.rigidbody = &userDataA->bodyEntity.GetComponent<Rigidbody2DComponent>();

            if (rb.OnCollisionExitCallback) {
                rb.OnCollisionExitCallback(callbackB);
            }
        }
        // ----------------------------------------------
    }
}
bool Physics2DPreSolve(b2ShapeId shapeIdA, b2ShapeId shapeIdB, b2Manifold* manifold, void* context)
{
    TB_CORE_ASSERT_TAGGED(b2Shape_IsValid(shapeIdA), "Shape is null!");
    TB_CORE_ASSERT_TAGGED(b2Shape_IsValid(shapeIdB), "Shape is null!");

    b2BodyId bodyIdA = b2Shape_GetBody(shapeIdA);
    b2BodyId bodyIdB = b2Shape_GetBody(shapeIdB);

    BodyUserData2D* userDataA = static_cast<BodyUserData2D*>(b2Body_GetUserData(bodyIdA));
    BodyUserData2D* userDataB = static_cast<BodyUserData2D*>(b2Body_GetUserData(bodyIdB));

    if (userDataA->bodyEntity.HasComponent<Rigidbody2DComponent>()) {
        auto& rb = userDataA->bodyEntity.GetComponent<Rigidbody2DComponent>();

        ContactCallback callbackA;
        callbackA.entity = userDataB->bodyEntity;
        callbackA.transform = &userDataB->bodyEntity.GetComponent<TransformComponent>();
        callbackA.rigidbody = &userDataB->bodyEntity.GetComponent<Rigidbody2DComponent>();

        if (rb.OnPreSolve) {
            return rb.OnPreSolve(callbackA);
        }
    }

    if (userDataB->bodyEntity.HasComponent<Rigidbody2DComponent>()) {
        auto& rb = userDataB->bodyEntity.GetComponent<Rigidbody2DComponent>();

        ContactCallback callbackB;
        callbackB.entity = userDataA->bodyEntity;
        callbackB.transform = &userDataA->bodyEntity.GetComponent<TransformComponent>();
        callbackB.rigidbody = &userDataA->bodyEntity.GetComponent<Rigidbody2DComponent>();

        if (rb.OnPreSolve) {
            return rb.OnPreSolve(callbackB);
        }
    }

    return true;
}
}
