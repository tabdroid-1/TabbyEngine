#include <Tabby.h>
#include <Tabby/Renderer/Mesh.h>

#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <box2d/box2d.h>

namespace Tabby {

World::World()
{
    TB_CORE_ASSERT_TAGGED(!s_Instance, "World already exists!");
    s_Instance = this;
}

void World::Init()
{
    if (!s_Instance)
        s_Instance = new World();

    AddSystem(Schedule::PreUpdate, [](entt::registry&) {
        // Apply transform to children
        TB_PROFILE_SCOPE_NAME("World::PreUpdate::TransformUpdate");
        auto view = World::GetRegistry().view<TransformComponent>();

        for (auto entity : view) {
            auto& hierarchy_node_component = World::GetRegistry().get<HierarchyNodeComponent>(entity);
            auto& transform = World::GetRegistry().get<TransformComponent>(entity);

            Matrix4 rotation = glm::toMat4(glm::quat(glm::radians((Vector3&)transform.Rotation)));
            transform.TransformMatrix = glm::translate(Matrix4(1.0f), (Vector3&)transform.Translation) * rotation * glm::scale(Matrix4(1.0f), (Vector3&)transform.Scale);

            Matrix4 localRotation = glm::toMat4(glm::quat(glm::radians((Vector3&)transform.LocalRotation)));
            transform.LocalTransformMatrix = glm::translate(Matrix4(1.0f), (Vector3&)transform.LocalTranslation) * localRotation * glm::scale(Matrix4(1.0f), (Vector3&)transform.LocalScale);

            for (auto& child : hierarchy_node_component.Children) {
                auto& childTransform = World::GetRegistry().get<TransformComponent>(child.second);
                childTransform.ApplyTransform(transform.GetTransform() * childTransform.GetLocalTransform());
            }

            if (World::GetRegistry().any_of<Rigidbody2DComponent>(entity)) {
                auto& rb2d = World::GetRegistry().get<Rigidbody2DComponent>(entity);

                if (B2_IS_NON_NULL(rb2d.RuntimeBodyId))
                    b2Body_SetTransform(rb2d.RuntimeBodyId, { transform.Translation.x, transform.Translation.y }, b2MakeRot(Math::DEG2RAD * transform.Rotation.z));
            }
        };
    });

    AddSystem(Schedule::PreUpdate, [](entt::registry&) {
        TB_PROFILE_SCOPE_NAME("World::PreUpdate::UpdatePhysics2DWorld");
        const int32_t subStepCount = 6;
        Physisc2D::UpdateWorld();

        // Retrieve transform from Box2D
        auto view = World::GetRegistry().view<Rigidbody2DComponent>();
        for (auto e : view) {
            Entity entity = { e };
            auto& transform = entity.GetComponent<TransformComponent>();
            auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

            const auto& position = b2Body_GetPosition(rb2d.RuntimeBodyId);
            transform.Translation.x = position.x;
            transform.Translation.y = position.y;
            transform.Rotation.z = Math::RAD2DEG * b2Rot_GetAngle(b2Body_GetRotation(rb2d.RuntimeBodyId));
        }
    });

    AddSystem(Schedule::PostUpdate, [](entt::registry&) {
        TB_PROFILE_SCOPE_NAME("World::PostUpdate::SetCurrentCamera");
        auto view = World::GetRegistry().view<TransformComponent, CameraComponent>();
        for (auto entity : view) {
            auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

            if (camera.Primary) {
                World::SetCurrentCamera(&camera.Camera, &transform.GetTransform());
                break;
            }
        }
    });

    AddSystem(Schedule::Draw, [](entt::registry&) {
        TB_PROFILE_SCOPE_NAME("World::Draw::RenderCircle");
        auto view = World::GetRegistry().view<TransformComponent, CircleRendererComponent>();
        for (auto entity : view) {
            auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);

            Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, (int)entity);
        }
    });

    AddSystem(Schedule::Draw, [](entt::registry&) {
        TB_PROFILE_SCOPE_NAME("World::Draw::RenderCircle");
        auto view = World::GetRegistry().view<TransformComponent, CircleRendererComponent>();
        for (auto entity : view) {
            auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);

            Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, (int)entity);
        }
    });

    AddSystem(Schedule::Draw, [](entt::registry&) {
        TB_PROFILE_SCOPE_NAME("World::Draw::RenderSprites");

        World::GetRegistry().sort<SpriteRendererComponent>([](const auto& lhs, const auto& rhs) {
            return lhs.renderOrder < rhs.renderOrder;
        });

        auto sprite_view = World::GetRegistry().view<SpriteRendererComponent>();

        for (const entt::entity entity : sprite_view) {
            auto transform = World::GetRegistry().get<TransformComponent>(entity);
            auto sprite = World::GetRegistry().get<SpriteRendererComponent>(entity);

            Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
        }
    });

    AddSystem(Schedule::Draw, [](entt::registry&) {
        TB_PROFILE_SCOPE_NAME("World::Draw::RenderGLTF");
        auto view = World::GetRegistry().view<TransformComponent, MeshComponent>();
        for (auto entity : view) {
            auto [transform, mC] = view.get<TransformComponent, MeshComponent>(entity);

            if (mC.m_Mesh) {
                mC.m_Mesh->SetTransform(transform.GetTransform());
                mC.m_Mesh->Render();
            }
        }
    });

    AddSystem(Schedule::Draw, [](entt::registry&) {
        TB_PROFILE_SCOPE_NAME("World::Draw::RenderText");

        auto view = GetRegistry().view<TransformComponent, TextComponent>();
        for (auto entity : view) {
            auto [transform, text] = view.get<TransformComponent, TextComponent>(entity);

            Renderer2D::DrawString(text.TextString, transform.GetTransform(), text, (int)entity);
        }
    });

    AddSystem(Schedule::Draw, [](entt::registry&) {
        TB_PROFILE_SCOPE_NAME("World::OnUpdate::RenderScene::DebugDraw");

        Debug::ProcessDrawCalls();
    });
}

template <typename... Component>
static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
{
    ([&]() {
        auto view = src.view<Component>();
        for (auto srcEntity : view) {
            entt::entity dstEntity = enttMap.at(src.get<IDComponent>(srcEntity).ID);

            auto& srcComponent = src.get<Component>(srcEntity);
            dst.emplace_or_replace<Component>(dstEntity, srcComponent);
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

void World::AddSystem(Schedule schedule, const std::function<void(entt::registry&)>& function)
{
    switch (schedule) {
    case Schedule::PreStartup:
        s_Instance->m_PreStartupSystems.push_back(function);
        break;
    case Schedule::Startup:
        s_Instance->m_StartupSystems.push_back(function);
        break;
    case Schedule::PostStartup:
        s_Instance->m_PostStartupSystems.push_back(function);
        break;

    case Schedule::PreUpdate:
        s_Instance->m_PreUpdateSystems.push_back(function);
        break;
    case Schedule::Update:
        s_Instance->m_UpdateSystems.push_back(function);
        break;
    case Schedule::PostUpdate:
        s_Instance->m_PostUpdateSystems.push_back(function);
        break;

    case Schedule::FixedPreUpdate:
        s_Instance->m_FixedPreUpdateSystems.push_back(function);
        break;
    case Schedule::FixedUpdate:
        s_Instance->m_FixedUpdateSystems.push_back(function);
        break;
    case Schedule::FixedPostUpdate:
        s_Instance->m_FixedPostUpdateSystems.push_back(function);
        break;

    case Schedule::Draw:
        s_Instance->m_DrawSystems.push_back(function);
        break;
    }
}

Entity World::CreateEntity(const std::string& name)
{
    return CreateEntityWithUUID(UUID(), name);
}

Entity World::CreateEntityWithUUID(UUID uuid, const std::string& name)
{
    Entity entity = { s_Instance->GetRegistry().create() };
    entity.AddComponent<IDComponent>(uuid);
    entity.AddComponent<TransformComponent>();
    entity.AddComponent<HierarchyNodeComponent>();
    auto& tag = entity.AddComponent<TagComponent>();
    tag.Tag = name.empty() ? "Entity" : name;

    s_Instance->m_EntityMap[uuid] = entity;

    return entity;
}

void World::DestroyEntity(Entity entity)
{
    if (entity.HasComponent<Rigidbody2DComponent>()) {
        auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

        if (B2_IS_NON_NULL(rb2d.RuntimeBodyId)) {
            b2DestroyBody(rb2d.RuntimeBodyId);
            rb2d.RuntimeBodyId = b2_nullBodyId;
        }
    }

    if (entity.HasComponent<NativeScriptComponent>()) {

        auto& nsc = entity.GetComponent<NativeScriptComponent>();

        nsc.DestroyScript(&nsc);
    }

    auto& hc = entity.GetComponent<HierarchyNodeComponent>();
    auto& tc = entity.GetComponent<TransformComponent>();
    auto& id_component = entity.GetComponent<IDComponent>();

    for (auto& child : hc.Children) {
        HierarchyNodeComponent& child_node_component = Entity(child.second).GetComponent<HierarchyNodeComponent>();
        child_node_component.Parent = hc.Parent.first == 0 ? std::make_pair(UUID(0), entt::null) : hc.Parent;
    }
    if (hc.Parent.first.Valid()) {
        Entity parent = Entity(hc.Parent.second);
        HierarchyNodeComponent& parent_node_component = parent.GetComponent<HierarchyNodeComponent>();

        for (auto& child : hc.Children)
            parent_node_component.Children.push_back(child);

        for (auto i = parent_node_component.Children.begin(); i != parent_node_component.Children.end(); i++) {
            if (i->first == id_component.ID) {
                parent_node_component.Children.erase(i);
                break;
            }
        }
    }
    s_Instance->m_EntityMap.erase(entity.GetUUID());
    GetRegistry().destroy(entity);
}

void World::DestroyEntityWithChildren(Entity entity)
{
    if (entity.HasComponent<Rigidbody2DComponent>()) {
        auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

        if (B2_IS_NON_NULL(rb2d.RuntimeBodyId)) {
            b2DestroyBody(rb2d.RuntimeBodyId);
            rb2d.RuntimeBodyId = b2_nullBodyId;
        }
    }

    if (entity.HasComponent<NativeScriptComponent>()) {

        auto& nsc = entity.GetComponent<NativeScriptComponent>();

        nsc.DestroyScript(&nsc);
    }

    IDComponent uuid_component = entity.GetComponent<IDComponent>();
    HierarchyNodeComponent& hierarchy_node_component = entity.GetComponent<HierarchyNodeComponent>();
    for (auto& child : hierarchy_node_component.Children) {
        DestroyEntityWithChildren(Entity(child.second));
    }
    DestroyEntity(entity);
}

Entity World::DuplicateEntity(Entity entity)
{
    // Copy name because we're going to modify component data structure
    std::string name = entity.GetName();
    Entity newEntity = CreateEntity(name);
    CopyComponentIfExists(AllComponents {}, newEntity, entity);
    return newEntity;
}

Entity World::FindEntityByName(std::string_view name)
{
    auto view = GetRegistry().view<TagComponent>();
    for (auto entity : view) {
        const TagComponent& tc = view.get<TagComponent>(entity);
        if (tc.Tag == name)
            return Entity { entity };
    }
    return {};
}

Entity World::GetEntityByUUID(UUID uuid)
{
    // TODO(Yan): Maybe should be assert
    if (s_Instance->m_EntityMap.find(uuid) != s_Instance->m_EntityMap.end())
        return { s_Instance->m_EntityMap.at(uuid) };

    return {};
}

Entity World::GetPrimaryCameraEntity()
{
    auto view = GetRegistry().view<CameraComponent>();
    for (auto entity : view) {
        const auto& camera = view.get<CameraComponent>(entity);
        if (camera.Primary)
            return Entity { entity };
    }
    return {};
}

void World::OnStart()
{
    TB_PROFILE_SCOPE_NAME("World::OnStart");

    if (!s_Instance)
        s_Instance = new World();

    s_Instance->m_IsRunning = true;

    Vector2 gravity(0.0f, -20.8f);
    Physisc2D::InitWorld(gravity);

    for (const auto& preStartup : s_Instance->m_PreStartupSystems)
        preStartup(s_Instance->m_EntityRegistry);

    for (const auto& startup : s_Instance->m_StartupSystems)
        startup(s_Instance->m_EntityRegistry);

    for (const auto& postStartup : s_Instance->m_PostStartupSystems)
        postStartup(s_Instance->m_EntityRegistry);
}

void World::OnStop()
{
    TB_PROFILE_SCOPE_NAME("World::OnStop");
    s_Instance->m_IsRunning = false;

    {
        auto view = GetRegistry().view<IDComponent>();
        for (auto entityHandle : view) {

            Entity entity = { entityHandle };
            if (entity.GetComponent<IDComponent>().IsPersistent)
                continue;

            DestroyEntity(entity);
        }
    }
}

void World::Update()
{
    if (!s_Instance->m_IsPaused || s_Instance->m_StepFrames-- > 0) {

        for (const auto& preUpdate : s_Instance->m_PreUpdateSystems)
            preUpdate(s_Instance->m_EntityRegistry);

        for (const auto& update : s_Instance->m_UpdateSystems)
            update(s_Instance->m_EntityRegistry);

        for (const auto& postUpdate : s_Instance->m_PostUpdateSystems)
            postUpdate(s_Instance->m_EntityRegistry);

        constexpr double fixedTimeStep = 1.0 / FIXED_UPDATE_RATE;

        s_Instance->m_FixedUpdateAccumulator += Time::GetDeltaTime();

        while (s_Instance->m_FixedUpdateAccumulator >= fixedTimeStep) {

            for (const auto& fixedPreUpdate : s_Instance->m_FixedPreUpdateSystems)
                fixedPreUpdate(s_Instance->m_EntityRegistry);

            for (const auto& fixedUpdate : s_Instance->m_FixedUpdateSystems)
                fixedUpdate(s_Instance->m_EntityRegistry);

            for (const auto& fixedPostUpdate : s_Instance->m_FixedPostUpdateSystems)
                fixedPostUpdate(s_Instance->m_EntityRegistry);

            s_Instance->m_FixedUpdateAccumulator -= fixedTimeStep;
        }
    }

    if (s_Instance->m_CurrentCamera) {

        Renderer2D::BeginScene(*s_Instance->m_CurrentCamera, *s_Instance->m_CurrentCameraTransform);

        for (const auto& draw : s_Instance->m_DrawSystems)
            draw(s_Instance->m_EntityRegistry);

        Renderer2D::EndScene();
    }
}

void World::DrawImGui()
{
    TB_PROFILE_SCOPE_NAME("World::DrawImGui");
}

void World::OnViewportResize(uint32_t width, uint32_t height)
{
    TB_PROFILE_SCOPE_NAME("World::OnViewportResize");

    if (s_Instance->m_ViewportWidth == width && s_Instance->m_ViewportHeight == height)
        return;

    s_Instance->m_ViewportWidth = width;
    s_Instance->m_ViewportHeight = height;

    // Resize our non-FixedAspectRatio cameras
    auto view = GetRegistry().view<CameraComponent>();
    for (auto entity : view) {
        auto& cameraComponent = view.get<CameraComponent>(entity);
        if (!cameraComponent.FixedAspectRatio)
            cameraComponent.Camera.SetViewportSize(width, height);
    }
}

void World::Step(int frames)
{
    s_Instance->m_StepFrames = frames;
}

void World::SetCurrentCamera(Camera* currentCamera, Matrix4* currentCameraTransform)
{
    s_Instance->m_CurrentCamera = currentCamera;
    s_Instance->m_CurrentCameraTransform = currentCameraTransform;
}

entt::registry& World::GetRegistry()
{
    return m_EntityRegistry;
}

template <typename T>
void World::OnComponentAdded(Entity entity, T& component)
{
    static_assert(sizeof(T) == 0);
}

template <>
void World::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component)
{
}

template <>
void World::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
{
}

template <>
void World::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
{
    if (s_Instance->m_ViewportWidth > 0 && s_Instance->m_ViewportHeight > 0)
        component.Camera.SetViewportSize(s_Instance->m_ViewportWidth, s_Instance->m_ViewportHeight);
}

template <>
void World::OnComponentAdded<SoundComponent>(Entity entity, SoundComponent& component)
{
}

template <>
void World::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
{
}

template <>
void World::OnComponentAdded<HierarchyNodeComponent>(Entity entity, HierarchyNodeComponent& component)
{
}

template <>
void World::OnComponentAdded<CircleRendererComponent>(Entity entity, CircleRendererComponent& component)
{
}

template <>
void World::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
{
}

template <>
void World::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
{
}

template <>
void World::OnComponentAdded<Rigidbody2DComponent>(Entity entity, Rigidbody2DComponent& component)
{
    if (B2_IS_NULL(Physisc2D::GetPhysicsWorld()))
        return;

    auto& transform = entity.GetComponent<TransformComponent>();

    if (B2_IS_NON_NULL(component.RuntimeBodyId))
        return;

    BodyInfo2D bodyInfo {
        entity,
    };

    Physisc2D::EnqueueBodyInit(bodyInfo);
}

template <>
void World::OnComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component)
{
    if (component.QueuedForInitialization || B2_IS_NON_NULL(component.RuntimeShapeId))
        return;

    component.worldId = Physisc2D::GetPhysicsWorld();

    ShapeInfo2D ShapeInfo = {
        entity,
        ColliderType2D::Box,
    };

    Physisc2D::EnqueueShapeInit(ShapeInfo);
    component.QueuedForInitialization = true;
}

template <>
void World::OnComponentAdded<CircleCollider2DComponent>(Entity entity, CircleCollider2DComponent& component)
{
    if (component.QueuedForInitialization || B2_IS_NON_NULL(component.RuntimeShapeId))
        return;

    component.WorldId = Physisc2D::GetPhysicsWorld();

    ShapeInfo2D shapeInfo {
        entity,
        ColliderType2D::Circle
    };

    Physisc2D::EnqueueShapeInit(shapeInfo);
    component.QueuedForInitialization = true;
}

template <>
void World::OnComponentAdded<CapsuleCollider2DComponent>(Entity entity, CapsuleCollider2DComponent& component)
{
    if (component.QueuedForInitialization || B2_IS_NON_NULL(component.RuntimeShapeId))
        return;

    component.WorldId = Physisc2D::GetPhysicsWorld();

    ShapeInfo2D shapeInfo {
        entity,
        ColliderType2D::Capsule
    };

    Physisc2D::EnqueueShapeInit(shapeInfo);
    component.QueuedForInitialization = true;
}

template <>
void World::OnComponentAdded<SegmentCollider2DComponent>(Entity entity, SegmentCollider2DComponent& component)
{
    if (component.QueuedForInitialization || B2_IS_NON_NULL(component.RuntimeShapeId))
        return;

    component.WorldId = Physisc2D::GetPhysicsWorld();

    ShapeInfo2D shapeInfo {
        entity,
        ColliderType2D::Segment
    };

    Physisc2D::EnqueueShapeInit(shapeInfo);
    component.QueuedForInitialization = true;
}

template <>
void World::OnComponentAdded<TextComponent>(Entity entity, TextComponent& component)
{
}

template <>
void World::OnComponentAdded<MeshComponent>(Entity entity, MeshComponent& component)
{
}
}
