#include "Scene.h"
#include "Entity.h"
#include "ScriptableEntity.h"
#include "glm/fwd.hpp"

#include "Components.h"
#include "Tabby/Physics/2D/Physics2D.h"
#include "Tabby/Renderer/Renderer2D.h"
// #include <Tabby/Audio/AudioManager.h>
// #include <Tabby/Audio/AudioSource.h>
#include <Tabby/Audio/AudioEngine.h>

#include <Tabby/Core/Log.h>
#include <Tabby/Math/Math.h>
#include <algorithm>
#include <cstdint>
#include <glm/glm.hpp>
#include <type_traits>

// Box2D
#include "box2d/b2_body.h"
#include "box2d/b2_circle_shape.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_settings.h"
#include "box2d/b2_world.h"

namespace Tabby {

Scene::Scene()
{
}

Scene::~Scene()
{
}

template <typename... Component>
static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
{
    // ([&]() {
    //     auto view = src.view<Component>();
    //     for (auto srcEntity : view) {
    //         entt::entity dstEntity = enttMap.at(src.get<IDComponent>(srcEntity).ID);
    //
    //         auto& srcComponent = src.get<Component>(srcEntity);
    //         dst.emplace_or_replace<Component>(dstEntity, srcComponent);
    //     }
    // }(),
    //     ...);
    ([&]() {
        auto view = src.view<Component>();
        for (auto srcEntity : view) {
            UUID srcUUID = src.get<IDComponent>(srcEntity).ID;
            if (enttMap.find(srcUUID) != enttMap.end()) {
                entt::entity dstEntity = enttMap.at(srcUUID);
                auto& srcComponent = src.get<Component>(srcEntity);
                dst.emplace_or_replace<Component>(dstEntity, srcComponent);

                if (std::is_same_v<Component, Rigidbody2DComponent>) {
                    auto& rb = dst.get<Rigidbody2DComponent>(dstEntity);

                    b2Body* body = (b2Body*)rb.RuntimeBody;
                    body->GetUserData().pointer = static_cast<std::uintptr_t>(dstEntity);
                }

                // if (std::is_same_v<Component, TransformComponent>) {

                // auto& transformComponent = dst.get<TransformComponent>(dstEntity);
                // for (auto& childTransform : transformComponent.Children) {
                //     childTransform.second = enttMap.find(childTransform.first)->second;
                // }
                //
                // if (transformComponent.Parent.first != NULL) {
                //     auto it = enttMap.find(transformComponent.Parent.first);
                //     if (it != enttMap.end()) {
                //         transformComponent.Parent.second = it->second;
                //     }
                // }
                // }
            }
        }
    }(),
        ...);
}

template <typename... Component>
static void CopyComponent(ComponentGroup<Component...>, entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
{
    CopyComponent<Component...>(dst, src, enttMap);
}

template <typename... Component>
static void CopyComponentIfExists(Entity dst, Entity src)
{
    ([&]() {
        if (src.HasComponent<Component>())
            dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
    }(),
        ...);
}

template <typename... Component>
static void CopyComponentIfExists(ComponentGroup<Component...>, Entity dst, Entity src)
{
    CopyComponentIfExists<Component...>(dst, src);
}

Ref<Scene> Scene::Copy(Ref<Scene> other)
{
    // Ref<Scene> newScene = CreateRef<Scene>();
    //
    // newScene->m_ViewportWidth = other->m_ViewportWidth;
    // newScene->m_ViewportHeight = other->m_ViewportHeight;
    //
    // auto& srcSceneRegistry = other->m_Registry;
    // auto& dstSceneRegistry = newScene->m_Registry;
    // std::unordered_map<UUID, entt::entity> enttMap;
    //
    // // Create entities in new scene
    // auto idView = srcSceneRegistry.view<IDComponent>();
    // for (auto e : idView) {
    //     UUID uuid = srcSceneRegistry.get<IDComponent>(e).ID;
    //     const auto& name = srcSceneRegistry.get<TagComponent>(e).Tag;
    //     Entity newEntity = newScene->CreateEntityWithUUID(uuid, name);
    //     enttMap[uuid] = (entt::entity)newEntity;
    // }
    //
    // // Copy components (except IDComponent and TagComponent)
    // CopyComponent(AllComponents {}, dstSceneRegistry, srcSceneRegistry, enttMap);
    //
    // return newScene;
}

void Scene::GetPersistentEntities(Ref<Scene> other)
{

    // auto& srcSceneRegistry = other->m_Registry;
    // std::unordered_map<UUID, entt::entity> enttMap;
    //
    // // Create entities in new scene
    // auto idView = srcSceneRegistry.view<IDComponent>();
    // for (auto e : idView) {
    //
    //     // If should be destroyed on load ignore and if it has Rigidbody2DComponent delete body from world.
    //     if (!srcSceneRegistry.get<IDComponent>(e).DoNotDestroyOnLoad) {
    //         if (srcSceneRegistry.any_of<Rigidbody2DComponent>(e)) {
    //             auto& rb = srcSceneRegistry.get<Rigidbody2DComponent>(e);
    //             if ((b2Body*)rb.RuntimeBody)
    //                 Physisc2D::GetPhysicsWorld()->DestroyBody((b2Body*)rb.RuntimeBody);
    //         }
    //         continue;
    //     }
    //
    //     UUID uuid = srcSceneRegistry.get<IDComponent>(e).ID;
    //     const auto& name = srcSceneRegistry.get<TagComponent>(e).Tag;
    //     Entity newEntity = CreateEntityWithUUID(uuid, name);
    //     enttMap[uuid] = (entt::entity)newEntity;
    // }
    //
    // // Copy components (except IDComponent and TagComponent)
    // CopyComponent(AllComponents {}, m_Registry, srcSceneRegistry, enttMap);
}

Entity Scene::CreateEntity(const std::string& name)
{
    return CreateEntityWithUUID(UUID(), name);
}

Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name)
{
    Entity entity = { SceneManager::GetRegistry().create() };
    entity.AddComponent<IDComponent>(uuid);
    entity.AddComponent<TransformComponent>();
    auto& tag = entity.AddComponent<TagComponent>();
    tag.Tag = name.empty() ? "Entity" : name;

    m_EntityMap[uuid] = entity;

    return entity;
}

void Scene::DestroyEntity(Entity entity)
{
    if (entity.HasComponent<Rigidbody2DComponent>()) {
        auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

        if (rb2d.RuntimeBody)
            Physisc2D::GetPhysicsWorld()->DestroyBody((b2Body*)rb2d.RuntimeBody);
    }

    if (entity.HasComponent<NativeScriptComponent>()) {

        auto& nsc = entity.GetComponent<NativeScriptComponent>();

        nsc.DestroyScript(&nsc);
    }
    m_EntityMap.erase(entity.GetUUID());
    SceneManager::GetRegistry().destroy(entity);
}

void Scene::OnStart()
{

    m_IsRunning = true;

    glm::vec2 gravity(0.0f, -20.8f);
    Physisc2D::InitWorld(gravity);

    OnActivate();
}

void Scene::OnStop()
{
    m_IsRunning = false;

    {
        auto view = SceneManager::GetRegistry().view<IDComponent>();
        for (auto entityHandle : view) {

            Entity entity = { entityHandle };
            if (entity.GetComponent<IDComponent>().IsPersistent)
                continue;

            DestroyEntity(entity);
        }
    }

    OnDeactivate();
}

void Scene::OnUpdate(Timestep ts)
{
    SceneManager::ProcessQueue(Physisc2D::IsQueueEmpty());

    if (!m_IsPaused || m_StepFrames-- > 0) {

        // Update scripts
        {
            SceneManager::GetRegistry().view<NativeScriptComponent>().each([=](auto entity, auto& nsc) {
                // TODO: Move to Scene::OnScenePlay
                if (!nsc.Instance) {
                    nsc.Instance = nsc.InstantiateScript();
                    nsc.Instance->m_Entity = Entity { entity };
                    nsc.Instance->OnCreate();
                }

                nsc.Instance->Update(ts);
                nsc.Instance->LateUpdate(ts);

                const double fixedTimeStep = 1.0 / FIXED_UPDATE_RATE;

                m_FixedUpdateAccumulator += ts;

                while (m_FixedUpdateAccumulator >= fixedTimeStep) {
                    nsc.Instance->FixedUpdate(fixedTimeStep);
                    m_FixedUpdateAccumulator -= fixedTimeStep;
                }
            });
        }
        // Physics
        {
            Physisc2D::ProcessBodyQueue();
            Physisc2D::ProcessFixtureQueue();

            const int32_t velocityIterations = 6;
            const int32_t positionIterations = 2;
            Physisc2D::UpdateWorld(ts, velocityIterations, positionIterations);

            // Retrieve transform from Box2D
            auto view = SceneManager::GetRegistry().view<Rigidbody2DComponent>();
            for (auto e : view) {
                Entity entity = { e };
                auto& transform = entity.GetComponent<TransformComponent>();
                auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
                b2Body* body = (b2Body*)rb2d.RuntimeBody;

                const auto& position = body->GetPosition();
                transform.Translation.x = position.x;
                transform.Translation.y = position.y;
                transform.Rotation.z = Math::RAD2DEG * body->GetAngle();
            }
        }

        // Apply transform to children
        {
            auto view = SceneManager::GetRegistry().view<TransformComponent>();

            for (auto entity : view) {
                auto& transform = SceneManager::GetRegistry().get<TransformComponent>(entity);

                for (auto& child : transform.Children) {
                    auto& childTransform = SceneManager::GetRegistry().get<TransformComponent>(child.second);
                    childTransform.ApplyTransform(transform.GetTransform() * childTransform.GetLocalTransform());
                }
            };
        }
    }

    // Sound
    {
        // Audio::Engine::polling_thread();
        auto view = SceneManager::GetRegistry().view<SoundComponent>();
        for (auto e : view) {
            Entity entity = { e };
            auto& sc = entity.GetComponent<SoundComponent>();

            // sc.Sound->SetGain(sc.Gain);

            // if (sc.Playing)
            //     sc.Sound->Play();
        }
    }

    // Render 2D
    Camera* mainCamera = nullptr;
    glm::mat4 cameraTransform;
    {
        auto view = SceneManager::GetRegistry().view<TransformComponent, CameraComponent>();
        for (auto entity : view) {
            auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

            if (camera.Primary) {
                mainCamera = &camera.Camera;
                cameraTransform = transform.GetTransform();
                break;
            }
        }
    }

    if (mainCamera) {
        Renderer2D::BeginScene(*mainCamera, cameraTransform);

        // Draw circles
        {
            auto view = SceneManager::GetRegistry().view<TransformComponent, CircleRendererComponent>();
            for (auto entity : view) {
                auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);

                Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, (int)entity);
            }
        }

        // Draw sprites
        {

            std::vector<entt::entity> sortedEntities;

            auto view = SceneManager::GetRegistry().view<SpriteRendererComponent>();
            for (auto entity : view) {
                sortedEntities.push_back(entity);
            }

            std::sort(sortedEntities.begin(), sortedEntities.end(), [this](const entt::entity& a, const entt::entity& b) {
                return SceneManager::GetRegistry().get<SpriteRendererComponent>(a).renderOrder < SceneManager::GetRegistry().get<SpriteRendererComponent>(b).renderOrder;
            });

            for (const entt::entity entity : sortedEntities) {
                auto transform = SceneManager::GetRegistry().get<TransformComponent>(entity);
                auto sprite = SceneManager::GetRegistry().get<SpriteRendererComponent>(entity);

                Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
            }
        }

        // Draw text
        {
            auto view = SceneManager::GetRegistry().view<TransformComponent, TextComponent>();
            for (auto entity : view) {
                auto [transform, text] = view.get<TransformComponent, TextComponent>(entity);

#if !defined TB_PLATFORM_WEB && !defined TB_PLATFORM_ANDROID
                Renderer2D::DrawString(text.TextString, transform.GetTransform(), text, (int)entity);
#endif
            }
        }

        Renderer2D::EndScene();
    }
}

void Scene::OnViewportResize(uint32_t width, uint32_t height)
{
    if (m_ViewportWidth == width && m_ViewportHeight == height)
        return;

    m_ViewportWidth = width;
    m_ViewportHeight = height;

    // Resize our non-FixedAspectRatio cameras
    auto view = SceneManager::GetRegistry().view<CameraComponent>();
    for (auto entity : view) {
        auto& cameraComponent = view.get<CameraComponent>(entity);
        if (!cameraComponent.FixedAspectRatio)
            cameraComponent.Camera.SetViewportSize(width, height);
    }
}

Entity Scene::GetPrimaryCameraEntity()
{
    auto view = SceneManager::GetRegistry().view<CameraComponent>();
    for (auto entity : view) {
        const auto& camera = view.get<CameraComponent>(entity);
        if (camera.Primary)
            return Entity { entity };
    }
    return {};
}

void Scene::Step(int frames)
{
    m_StepFrames = frames;
}

Entity Scene::DuplicateEntity(Entity entity)
{
    // Copy name because we're going to modify component data structure
    std::string name = entity.GetName();
    Entity newEntity = CreateEntity(name);
    CopyComponentIfExists(AllComponents {}, newEntity, entity);
    return newEntity;
}

Entity Scene::FindEntityByName(std::string_view name)
{
    auto view = SceneManager::GetRegistry().view<TagComponent>();
    for (auto entity : view) {
        const TagComponent& tc = view.get<TagComponent>(entity);
        if (tc.Tag == name)
            return Entity { entity };
    }
    return {};
}

Entity Scene::GetEntityByUUID(UUID uuid)
{
    // TODO(Yan): Maybe should be assert
    if (m_EntityMap.find(uuid) != m_EntityMap.end())
        return { m_EntityMap.at(uuid) };

    return {};
}

template <typename T>
void Scene::OnComponentAdded(Entity entity, T& component)
{
    static_assert(sizeof(T) == 0);
}

template <>
void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component)
{
}

template <>
void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
{
}

template <>
void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
{
    if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
        component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
}

template <>
void Scene::OnComponentAdded<SoundComponent>(Entity entity, SoundComponent& component)
{
}

template <>
void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
{
}

template <>
void Scene::OnComponentAdded<CircleRendererComponent>(Entity entity, CircleRendererComponent& component)
{
}

template <>
void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
{
}

template <>
void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
{
}

template <>
void Scene::OnComponentAdded<Rigidbody2DComponent>(Entity entity, Rigidbody2DComponent& component)
{

    if (Physisc2D::GetPhysicsWorld()) {

        auto& transform = entity.GetComponent<TransformComponent>();
        auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

        if (rb2d.initialized)
            return;

        b2BodyUserData bodyData;
        bodyData.pointer = static_cast<std::uintptr_t>(entity.GetEntityHandle());

        b2BodyDef* bodyDef = new b2BodyDef;
        bodyDef->type = Utils::Rigidbody2DTypeToBox2DBody(rb2d.Type);
        bodyDef->position.Set(transform.Translation.x, transform.Translation.y);
        bodyDef->angle = transform.Rotation.z;
        bodyDef->userData = bodyData;

        Physisc2D::EnqueueBody(entity, bodyDef);
        // b2Body* body = Physisc2D::GetPhysicsWorld()->CreateBody(&bodyDef);
        // body->SetFixedRotation(rb2d.FixedRotation);

        // TODO: Move this to OnComponentAdded.
        if (entity.HasComponent<BoxCollider2DComponent>()) {
            auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();

            b2PolygonShape* boxShape = new b2PolygonShape;
            boxShape->SetAsBox(bc2d.Size.x * transform.Scale.x, bc2d.Size.y * transform.Scale.y, b2Vec2(bc2d.Offset.x, bc2d.Offset.y), 0.0f);

            b2FixtureDef* fixtureDef = new b2FixtureDef;
            fixtureDef->shape = boxShape;
            fixtureDef->density = bc2d.Density;
            fixtureDef->friction = bc2d.Friction;
            fixtureDef->restitution = bc2d.Restitution;
            fixtureDef->restitutionThreshold = bc2d.RestitutionThreshold;
            Physisc2D::EnqueueFixture(entity, fixtureDef);
            // body->CreateFixture(&fixtureDef);
        }

        if (entity.HasComponent<CircleCollider2DComponent>()) {
            auto& cc2d = entity.GetComponent<CircleCollider2DComponent>();

            b2CircleShape* circleShape = new b2CircleShape;
            circleShape->m_p.Set(cc2d.Offset.x, cc2d.Offset.y);
            circleShape->m_radius = transform.Scale.x * cc2d.Radius;

            b2FixtureDef* fixtureDef = new b2FixtureDef;
            fixtureDef->shape = circleShape;
            fixtureDef->density = cc2d.Density;
            fixtureDef->friction = cc2d.Friction;
            fixtureDef->restitution = cc2d.Restitution;
            fixtureDef->restitutionThreshold = cc2d.RestitutionThreshold;
            Physisc2D::EnqueueFixture(entity, fixtureDef);
            // body->CreateFixture(&fixtureDef);
        }

        // rb2d.RuntimeBody = body;
        rb2d.initialized = true;

        // auto& transform = entity.GetComponent<TransformComponent>();
        // auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
        //
        // b2BodyUserData bodyData;
        //
        // entt::entity e = entity.GetEntityHandle();
        // // bodyData.pointer = reinterpret_cast<uintptr_t>(&entity);
        // bodyData.pointer = static_cast<std::uintptr_t>(e);
        //
        // b2BodyDef bodyDef;
        // bodyDef.type = Utils::Rigidbody2DTypeToBox2DBody(rb2d.Type);
        // bodyDef.position.Set(transform.Translation.x, transform.Translation.y);
        // bodyDef.angle = transform.Rotation.z;
        // bodyDef.userData = bodyData;
        //
        // b2Body* body = Physisc2D::GetPhysicsWorld()->CreateBody(&bodyDef);
        // body->SetFixedRotation(rb2d.FixedRotation);
        //
        // if (entity.HasComponent<BoxCollider2DComponent>()) {
        //     auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
        //
        //     b2PolygonShape boxShape;
        //     boxShape.SetAsBox(bc2d.Size.x * transform.Scale.x, bc2d.Size.y * transform.Scale.y, b2Vec2(bc2d.Offset.x, bc2d.Offset.y), 0.0f);
        //
        //     b2FixtureDef fixtureDef;
        //     fixtureDef.shape = &boxShape;
        //     fixtureDef.density = bc2d.Density;
        //     fixtureDef.friction = bc2d.Friction;
        //     fixtureDef.restitution = bc2d.Restitution;
        //     fixtureDef.restitutionThreshold = bc2d.RestitutionThreshold;
        //     body->CreateFixture(&fixtureDef);
        // }
        //
        // if (entity.HasComponent<CircleCollider2DComponent>()) {
        //     auto& cc2d = entity.GetComponent<CircleCollider2DComponent>();
        //
        //     b2CircleShape circleShape;
        //     circleShape.m_p.Set(cc2d.Offset.x, cc2d.Offset.y);
        //     circleShape.m_radius = transform.Scale.x * cc2d.Radius;
        //
        //     b2FixtureDef fixtureDef;
        //     fixtureDef.shape = &circleShape;
        //     fixtureDef.density = cc2d.Density;
        //     fixtureDef.friction = cc2d.Friction;
        //     fixtureDef.restitution = cc2d.Restitution;
        //     fixtureDef.restitutionThreshold = cc2d.RestitutionThreshold;
        //     body->CreateFixture(&fixtureDef);
        // }
        //
        // rb2d.RuntimeBody = body;
        // rb2d.initialized = true;
    }
}

template <>
void Scene::OnComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component)
{
}

template <>
void Scene::OnComponentAdded<CircleCollider2DComponent>(Entity entity, CircleCollider2DComponent& component)
{
}

template <>
void Scene::OnComponentAdded<TextComponent>(Entity entity, TextComponent& component)
{
}
}
