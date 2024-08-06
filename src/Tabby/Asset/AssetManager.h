#pragma once

#include <tbpch.h>
#include <Tabby/Asset/AssetBase.h>

namespace Tabby {

class AssetManager {
public:
    AssetManager();
    ~AssetManager();

    static void Init();
    static void Shutdown();
    void FullUnload();

    static AssetManager* Get() { return s_Instance; }

    AssetHandle LoadAssetSource(std::filesystem::path path, const AssetHandle& id = AssetHandle());
    AssetHandle RegisterAsset(Shared<AssetBase> asset, const AssetHandle& id = AssetHandle());
    bool HasAsset(AssetHandle id) { return m_AssetRegistry.find(id) != m_AssetRegistry.end(); }

    template <typename ResourceType> // where ResourceType is child of AssetType: Image, Mesh, Material etc.
    Shared<ResourceType> GetAsset(AssetHandle id)
    {
        return std::static_pointer_cast<ResourceType>(m_AssetRegistry.at(id));
    }
    AssetHandle GetHandle(std::filesystem::path path) const { return m_UUIDs.at(path.string()); }
    auto* GetAssetRegistry() const { return &m_AssetRegistry; }

private:
    AssetHandle ImportMeshSource(std::filesystem::path path, AssetHandle handle);
    AssetHandle ImportImageSource(std::filesystem::path path, AssetHandle handle);
    AssetHandle ImportFontSource(std::filesystem::path path, AssetHandle handle);

private:
    inline static AssetManager* s_Instance;
    std::unordered_map<AssetHandle, Shared<AssetBase>> m_AssetRegistry;
    std::unordered_map<std::string, UUID> m_UUIDs;
    std::shared_mutex m_Mutex;
};

}
