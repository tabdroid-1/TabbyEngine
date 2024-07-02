#pragma once

#include <Tabby/Asset/AssetBase.h>

namespace Tabby {

class Entity;

class Prefab : public AssetBase {
public:
    Prefab(AssetHandle handle)
        : m_RootEntityData(this)
    {
        Handle = handle;
        Type = AssetType::TABBY_PREFAB;
    };

    virtual ~Prefab() = default;

    template <typename... Components>
    static void RegisterComponents()
    {
        (RegisterComponent<Components>(), ...);
    }

    template <typename T>
    static void RegisterComponent()
    {
        m_TypeAddComponentMap.emplace(typeid(T), AddComponent<T>);
        m_TypeMap.emplace(typeid(T).name(), std::type_index(typeid(T)));
    }

    Entity Instantiate() const;

    static std::vector<uint8_t> SerializePrefab(Shared<Prefab> prefab);
    static Shared<Prefab> DeserializePrefab(AssetHandle handle, const std::vector<uint8_t>& data);

    static void SerializePrefabToFile(Shared<Prefab> prefab, const std::string& filePath);

    void Destroy() override
    {
    }

protected:
    struct ComponentData {
        std::type_index type;
        std::vector<uint8_t> data;
    };

    class EntityData {
    public:
        EntityData()
            : m_UUID(UUID())
            , m_Name("Prefab")
        {
        }
        EntityData(Prefab* prefab)
            : m_UUID(UUID())
            , m_Name("Prefab")
        {
        }

        template <typename T, typename... Args>
        T& AddComponent(T component)
        {
            m_Components.emplace_back(ComponentData { typeid(T), SerializeComponent<T>(component) });
            return component;
        }

        EntityData& AddChild();

        void SetName(const std::string& name) { m_Name = name; }

        static std::vector<uint8_t> SerializeEntityData(EntityData entityData);

        static EntityData DeserializeEntityData(const std::vector<uint8_t>& data, size_t& offset);

    private:
        template <typename T>
        std::vector<uint8_t> SerializeComponent(const T& component)
        {
            size_t size = sizeof(T);
            std::vector<uint8_t> data(size);
            std::memcpy(data.data(), &component, size);
            return data;
        }

        template <typename T>
        T DeserializeComponent(const std::vector<uint8_t>& data) const
        {
            if (data.size() != sizeof(T)) {
                throw std::runtime_error("Invalid data size for component");
            }
            T component;
            std::memcpy(&component, data.data(), data.size());
            return component;
        }

    private:
        UUID m_UUID;
        std::string m_Name;
        std::vector<ComponentData> m_Components;
        std::unordered_map<UUID, EntityData> m_ChildMapping;

        friend class Prefab;
    };

    EntityData& GetRootEntity();

private:
    Entity CreateEntityFromPrefab(EntityData entityData) const;

    static void InitializeStandardTypes();

    template <typename T>
    static void AddComponent(Entity& newEntity, const EntityData& entityData, const std::vector<uint8_t>& componentData);

private:
    EntityData m_RootEntityData;

    inline static std::unordered_map<std::string, std::type_index> m_TypeMap;
    inline static std::unordered_map<std::type_index, std::function<void(Entity& newEntity, const EntityData& entityData, const std::vector<uint8_t>& componentData)>> m_TypeAddComponentMap;

    friend class World;
};

}
