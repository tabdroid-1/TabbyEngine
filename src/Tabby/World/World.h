#pragma once

#include <tbpch.h>
#include <Tabby/Core/Time/Timestep.h>

#include <entt.hpp>

namespace Tabby {

class Camera;
class Entity;

enum Schedule : uint8_t {
    PreStartup = 0,
    Startup,
    PostStartup,

    // First,
    PreUpdate,
    Update,
    PostUpdate,
    // Last,

    // FixedFirst,
    FixedPreUpdate,
    FixedUpdate,
    FixedPostUpdate,
    // FixedLast,

    Draw,
};

class World {

public:
    World();

    static void Init();

    template <typename... PluginTypes>
    static void AddPlugins()
    {

        ([&]() {
            auto var = new PluginTypes();
            var->Build();
            delete var;
        }(),
            ...);
    }
    static void AddSystem(Schedule schedule, const std::function<void(entt::registry&)>& function);

    static Entity CreateEntity(const std::string& name = std::string());
    static Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
    static void DestroyEntity(Entity entity);
    static void DestroyEntityWithChildren(Entity entity);

    static Entity DuplicateEntity(Entity entity);

    static Entity FindEntityByName(std::string_view name);
    static Entity GetEntityByUUID(UUID uuid);

    static Entity GetPrimaryCameraEntity();

    static void OnStart();
    static void OnStop();

    static void Update(Timestep ts);
    static void DrawImGui();

    static void OnViewportResize(uint32_t width, uint32_t height);

    static bool IsRunning() { return s_Instance->m_IsRunning; }
    static bool IsPaused() { return s_Instance->m_IsPaused; }

    static void SetPaused(bool paused) { s_Instance->m_IsPaused = paused; }

    static void Step(int frames = 1);

    template <typename... Components>
    static auto GetAllEntitiesWith()
    {
        return s_Instance->GetRegistry().view<Components...>();
    }

    static entt::registry& GetRegistry();

    static void SetCurrentCamera(Camera* currentCamera, glm::mat4* currentCameraTransform);

private:
    template <typename T>
    static void OnComponentAdded(Entity entity, T& component);

    template <typename T>
    void ProcessPlugin()
    {
    }

private:
    uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
    bool m_IsRunning = false;
    bool m_IsPaused = false;
    int m_StepFrames = 0;

    float m_FixedUpdateAccumulator = 0;

    Camera* m_CurrentCamera;
    const glm::mat4* m_CurrentCameraTransform;

    inline static entt::registry m_EntityRegistry;
    std::unordered_map<UUID, entt::entity> m_EntityMap;

    // Called onece on application startup
    std::vector<std::function<void(entt::registry&)>> m_PreStartupSystems;
    std::vector<std::function<void(entt::registry&)>> m_StartupSystems;
    std::vector<std::function<void(entt::registry&)>> m_PostStartupSystems;

    // Called every frame
    // std::vector<std::function<void(entt::registry&)>> m_FirstSystems;
    std::vector<std::function<void(entt::registry&)>> m_PreUpdateSystems;
    std::vector<std::function<void(entt::registry&)>> m_UpdateSystems;
    std::vector<std::function<void(entt::registry&)>> m_PostUpdateSystems;
    // std::vector<std::function<void(entt::registry&)>> m_LastSystems;

    // Called fixed update
    // std::vector<std::function<void(entt::registry&)>> m_FixedFirstSystems;
    std::vector<std::function<void(entt::registry&)>> m_FixedPreUpdateSystems;
    std::vector<std::function<void(entt::registry&)>> m_FixedUpdateSystems;
    std::vector<std::function<void(entt::registry&)>> m_FixedPostUpdateSystems;
    // std::vector<std::function<void(entt::registry&)>> m_FixedLastSystems;

    // Called everyupdate
    std::vector<std::function<void(entt::registry&)>> m_DrawSystems;

private:
    inline static World* s_Instance;

    friend class Entity;
};

}