#include "box2d/b2_math.h"
#include "raylib.h"
#include <Physics/Physics.h>
#include <box2d/box2d.h>

b2World* Physics::physicsWorld = nullptr;

Physics::Physics()
    : velocityIterations(6)
    , positionIterations(2)
{
}

void Physics::Load(Vector2 Gravity)
{
    b2Vec2 gravity(Gravity.x, Gravity.y);
    physicsWorld = new b2World(gravity);
}

void Physics::Load(Vector2 Gravity, int VelocityIterations, int PositionIterations)
{
    velocityIterations = VelocityIterations;
    positionIterations = PositionIterations;
    b2Vec2 gravity(Gravity.x, Gravity.y);
    physicsWorld = new b2World(gravity);
}

void Physics::Update(float dt)
{
    physicsWorld->Step(dt, velocityIterations, positionIterations);
}

void Physics::Draw(Camera2D& camera)
{
    DrawColliders(physicsWorldScale, camera);
}

void Physics::Destroy()
{
    delete physicsWorld;
}

void Physics::DrawColliders(float worldScale, Camera2D& camera)
{
    auto currentBody = physicsWorld->GetBodyList();
    while (currentBody != nullptr) {
        auto pos = currentBody->GetPosition();
        auto angle = currentBody->GetAngle();

        DrawCircle(pos.x * worldScale,
            pos.y * worldScale,
            5,
            GREEN);

        auto fixture = currentBody->GetFixtureList();
        while (fixture != nullptr) {
            auto shape = fixture->GetShape();
            // Note, right now supposing all shapes are polygons, use to determine shape->GetType();

            if (shape->GetType() == b2Shape::e_circle) {

                b2CircleShape* circleShape = static_cast<b2CircleShape*>(shape);
                b2Vec2 position = currentBody->GetPosition();
                b2Vec2 offset = b2Mul(b2Rot(angle), circleShape->m_p);
                DrawCircleLines((position.x + offset.x) * worldScale, (position.y + offset.y) * worldScale, circleShape->m_radius * worldScale, GREEN);

            } else if (shape->GetType() == b2Shape::e_polygon) {
                auto polygonShape = (b2PolygonShape*)shape;
                int vertexCount = polygonShape->m_count;
                for (int j = 0; j < vertexCount; j++) {
                    b2Vec2 vertexA = polygonShape->m_vertices[j];
                    b2Vec2 vertexB = polygonShape->m_vertices[(j + 1) % vertexCount];

                    // Rotate the vertices based on the body's angle
                    b2Vec2 rotatedVertexA = b2Mul(b2Rot(angle), vertexA);
                    b2Vec2 rotatedVertexB = b2Mul(b2Rot(angle), vertexB);

                    DrawLineV({ (pos.x + rotatedVertexA.x) * worldScale, (pos.y + rotatedVertexA.y) * worldScale },
                        { (pos.x + rotatedVertexB.x) * worldScale, (pos.y + rotatedVertexB.y) * worldScale }, GREEN);
                }
            }

            fixture = fixture->GetNext();
        }

        currentBody = currentBody->GetNext();
    }
}

Vector2 Physics::ConvertWorldToScreen(const b2Vec2& worldPosition, const Vector2& windowSize, const float worldScale)
{
    float x = worldPosition.x * worldScale;
    float y = windowSize.y - (worldPosition.y * worldScale); // Invert the y-axis for Raylib

    return { x, -y };
}