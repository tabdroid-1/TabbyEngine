#include <Tabby/Asset/GLTFLoader.h>
#include <Tabby/Renderer/Texture.h>
#include <Tabby/Renderer/Mesh.h>
#include <Tabby/Renderer/Material.h>
#include <Tabby/World/Entity.h>
#include <Tabby/World/World.h>
#include <Tabby/Asset/AssetFile.h>
#include <Tabby/Asset/AssetManager.h>
#include <Tabby/Renderer/Buffer.h>
#include <Tabby/Renderer/VertexArray.h>
#include <Tabby/Renderer/RenderCommand.h>
#include <Tabby/Renderer/Renderer.h>

#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>
#include <fastgltf/tools.hpp>
#include <stb_image.h>

#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>
#include <fastgltf/tools.hpp>

#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Tabby {

void GLTFLoader::Parse(const std::filesystem::path& filePath)
{

    fastgltf::Asset m_Asset;
    std::vector<Shared<Texture>> m_Images;
    std::vector<MaterialUniforms> m_Materials;

    std::string err;
    std::string warn;

    fastgltf::Parser parser;

    constexpr auto gltfOptions = fastgltf::Options::DontRequireValidAssetMember | fastgltf::Options::AllowDouble | fastgltf::Options::LoadGLBBuffers | fastgltf::Options::LoadExternalBuffers | fastgltf::Options::LoadExternalImages | fastgltf::Options::GenerateMeshIndices;

#ifdef TB_PLATFORM_ANDROID
    auto asset = parser.loadFileFromApk(filePath);
#else
    auto gltfFile = fastgltf::MappedGltfFile::FromPath(filePath);

    std::string message = "Failed to load glTF file: " + std::string(fastgltf::getErrorMessage(gltfFile.error()));
    TB_CORE_ASSERT_TAGGED(bool(gltfFile), message);
    auto asset = parser.loadGltf(gltfFile.get(), filePath.parent_path(), gltfOptions);
#endif

    message = "Failed to load glTF file: " + std::string(fastgltf::getErrorMessage(asset.error()));
    TB_CORE_ASSERT_TAGGED(asset.error() == fastgltf::Error::None, message);

    m_Asset = std::move(asset.get());
    auto& defaultMaterial = m_Materials.emplace_back();

    LoadImages(m_Asset, m_Images);
    LoadMaterials(m_Asset, m_Materials);
    LoadMeshes(m_Asset, m_Images, m_Materials);
}

void GLTFLoader::LoadImages(fastgltf::Asset& asset, std::vector<Shared<Texture>>& images)
{
    auto getLevelCount = [](int width, int height) -> int {
        return static_cast<int>(1 + floor(log2(width > height ? width : height)));
    };

    for (auto& image : asset.images) {
        Shared<Texture> imageptr;
        std::visit(fastgltf::visitor {
                       [](auto& arg) {},
                       [&](fastgltf::sources::URI& filePath) {
                           TB_CORE_ASSERT_TAGGED(filePath.fileByteOffset == 0, "File offsets not supported");
                           TB_CORE_ASSERT_TAGGED(filePath.uri.isLocalPath(), "Only local files allowed"); // We're only capable of loading local files.

                           const std::string path(filePath.uri.path());

                           AssetHandle handle = AssetManager::Get()->LoadAssetSource(path, handle);
                           imageptr = AssetManager::Get()->GetAsset<Texture>(handle);
                       },
                       [&](fastgltf::sources::Array& vector) {
                           Buffer data;
                           int image_width, image_height, channels;

                           stbi_set_flip_vertically_on_load(false);
                           data.Data = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(vector.bytes.data()), static_cast<int>(vector.bytes.size()), &image_width, &image_height, &channels, 4);

                           data.Size = image_width * image_height * channels;
                           if (data.Data == nullptr) {
                               TB_CORE_ERROR("TextureImporter::ImportTexture - Could not load texture from data");
                           }

                           TextureSpecification texture_spec = {};
                           texture_spec.Format = ImageFormat::RGBA8;
                           texture_spec.Width = image_width;
                           texture_spec.Height = image_height;
                           texture_spec.type = ImageType::TYPE_2D;
                           texture_spec.usage = ImageUsage::TEXTURE;
                           texture_spec.array_layers = 1;
                           texture_spec.GenerateMips = true;

                           AssetHandle handle;
                           imageptr = Texture::Create(texture_spec, handle, data);
                           data.Release();
                       },
                       [&](fastgltf::sources::BufferView& view) {
                           auto& bufferView = asset.bufferViews[view.bufferViewIndex];
                           auto& buffer = asset.buffers[bufferView.bufferIndex];
                           std::visit(fastgltf::visitor {
                                          [](auto& arg) {},
                                          [&](fastgltf::sources::Array& vector) {
                                              Buffer data;
                                              int image_width, image_height, channels;

                                              stbi_set_flip_vertically_on_load(false);
                                              data.Data = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(vector.bytes.data() + bufferView.byteOffset),
                                                  static_cast<int>(bufferView.byteLength), &image_width, &image_height, &channels, 4);

                                              data.Size = image_width * image_height * channels;
                                              if (data.Data == nullptr) {
                                                  TB_CORE_ERROR("TextureImporter::ImportTexture - Could not load texture from data");
                                              }

                                              TextureSpecification texture_spec = {};
                                              texture_spec.Format = ImageFormat::RGBA8;
                                              texture_spec.Width = image_width;
                                              texture_spec.Height = image_height;
                                              texture_spec.type = ImageType::TYPE_2D;
                                              texture_spec.usage = ImageUsage::TEXTURE;
                                              texture_spec.array_layers = 1;
                                              texture_spec.GenerateMips = true;

                                              AssetHandle handle;
                                              imageptr = Texture::Create(texture_spec, handle, data);
                                              data.Release();
                                          } },
                               buffer.data);
                       },
                   },
            image.data);

        images.emplace_back(imageptr);
    }
}

void GLTFLoader::LoadMaterials(fastgltf::Asset& asset, std::vector<MaterialUniforms>& m_Materials)
{
    for (auto& material : asset.materials) {

        auto uniforms = m_Materials.emplace_back();
        uniforms.alphaCutoff = material.alphaCutoff;

        uniforms.baseColorFactor = Vector4(material.pbrData.baseColorFactor.x(), material.pbrData.baseColorFactor.y(), material.pbrData.baseColorFactor.z(), material.pbrData.baseColorFactor.w());
        if (material.pbrData.baseColorTexture.has_value())
            uniforms.flags |= MaterialUniformFlags::HasAlbedoMap;

        if (material.normalTexture.has_value())
            uniforms.flags |= MaterialUniformFlags::HasNormalMap;

        if (material.pbrData.metallicRoughnessTexture.has_value())
            uniforms.flags |= MaterialUniformFlags::HasRoughnessMap;

        if (material.occlusionTexture.has_value())
            uniforms.flags |= MaterialUniformFlags::HasOcclusionMap;

        m_Materials.emplace_back(uniforms);
    }
}

void GLTFLoader::LoadMeshes(fastgltf::Asset& asset, std::vector<Shared<Texture>>& images, std::vector<MaterialUniforms>& materials)
{

    std::vector<Shared<Tabby::Mesh>> tabbyMeshes;

    for (auto& mesh : asset.meshes) {

        for (auto it = mesh.primitives.begin(); it != mesh.primitives.end(); ++it) {

            auto tabbyMesh = CreateShared<Mesh>();
            tabbyMesh->SetName(mesh.name.c_str());
            auto* positionIt = it->findAttribute("POSITION");
            TB_CORE_ASSERT(positionIt != it->attributes.end()); // A mesh primitive is required to hold the POSITION attribute.
            TB_CORE_ASSERT(it->indicesAccessor.has_value()); // We specify GenerateMeshIndices, so we should always have indices

            auto index = std::distance(mesh.primitives.begin(), it);
            // auto& primitive = .primitives[index];
            tabbyMesh->SetPrimitiveType((Mesh::PrimitiveType)fastgltf::to_underlying(it->type));

            std::size_t materialUniformsIndex;
            std::size_t baseColorTexcoordIndex;

            Shared<Material> tabbyMaterial;
            if (Renderer::GetAPI() == RendererAPI::API::OpenGL46)
                tabbyMaterial = CreateShared<Material>("UnlitMaterial", "shaders/gl46/Renderer3D_MeshUnlit.glsl");
            else if (Renderer::GetAPI() == RendererAPI::API::OpenGL33)
                tabbyMaterial = CreateShared<Material>("UnlitMaterial", "shaders/gl33/Renderer3D_MeshUnlit.glsl");
            else if (Renderer::GetAPI() == RendererAPI::API::OpenGLES3)
                tabbyMaterial = CreateShared<Material>("UnlitMaterial", "shaders/gles3/Renderer3D_MeshUnlit.glsl");

            if (it->materialIndex.has_value()) {
                materialUniformsIndex = it->materialIndex.value() + 1; // Adjust for default material
                auto& material = asset.materials[it->materialIndex.value()];

                auto& baseColorTexture = material.pbrData.baseColorTexture;
                if (baseColorTexture.has_value()) {
                    auto& texture = asset.textures[baseColorTexture->textureIndex];
                    if (!texture.imageIndex.has_value())
                        return;

                    auto test = images[texture.imageIndex.value()];
                    tabbyMaterial->SetAlbedoMap(images[texture.imageIndex.value()]);

                    if (baseColorTexture->transform && baseColorTexture->transform->texCoordIndex.has_value()) {
                        baseColorTexcoordIndex = baseColorTexture->transform->texCoordIndex.value();
                    } else {
                        baseColorTexcoordIndex = material.pbrData.baseColorTexture->texCoordIndex;
                    }
                }
            } else {
                materialUniformsIndex = 0;
            }

            std::vector<Mesh::Vertex> meshVertices;
            {
                // Position
                auto& positionAccessor = asset.accessors[positionIt->accessorIndex];
                if (!positionAccessor.bufferViewIndex.has_value())
                    continue;

                meshVertices.resize(positionAccessor.count);

                fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(asset, positionAccessor, [&](fastgltf::math::fvec3 pos, std::size_t idx) {
                    meshVertices[idx].Position = Vector4(pos.x(), pos.y(), pos.z(), 0.0f);
                });
            }

            auto texcoordAttribute = std::string("TEXCOORD_") + std::to_string(baseColorTexcoordIndex);
            if (const auto* texcoord = it->findAttribute(texcoordAttribute); texcoord != it->attributes.end()) {
                // Tex coord
                auto& texCoordAccessor = asset.accessors[texcoord->accessorIndex];
                if (!texCoordAccessor.bufferViewIndex.has_value())
                    continue;

                fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec2>(asset, texCoordAccessor, [&](fastgltf::math::fvec2 uv, std::size_t idx) {
                    meshVertices[idx].TexCoords = Vector2(uv.x(), uv.y());
                });
            }

            auto& indexAccessor = asset.accessors[it->indicesAccessor.value()];
            if (!indexAccessor.bufferViewIndex.has_value())
                return;
            std::vector<uint32_t> meshIndices(indexAccessor.count);

            fastgltf::copyFromAccessor<std::uint32_t>(asset, indexAccessor, (std::uint32_t*)meshIndices.data());

            tabbyMesh->SetMaterial(tabbyMaterial);
            tabbyMesh->SetVertices(meshVertices);
            tabbyMesh->SetIndices(meshIndices);

            tabbyMesh->Create();
            tabbyMeshes.push_back(tabbyMesh);
        }
    }

    auto SceneEntity = Tabby::World::CreateEntity("Scene");

    for (auto& node : asset.nodes) {

        auto ent = Tabby::World::CreateEntity(node.name.c_str());
        auto& mC = ent.AddComponent<MeshComponent>();
        mC.m_Mesh = tabbyMeshes[node.meshIndex.value()];

        auto& tc = ent.GetComponent<TransformComponent>();
        std::visit(fastgltf::visitor { [&](const fastgltf::math::fmat4x4& matrix) {
                                          fastgltf::math::fvec3 scale;
                                          fastgltf::math::fquat rotation;
                                          fastgltf::math::fvec3 translation;
                                          fastgltf::math::decomposeTransformMatrix(matrix, scale, rotation, translation);

                                          Vector3 Gscale = { scale.x(), scale.y(), scale.z() };
                                          Quaternion Grotation = { rotation.w(), rotation.x(), rotation.y(), rotation.z() };
                                          Vector3 Gtranslation = { translation.x(), translation.y(), translation.z() };
                                          Matrix4 rotMat = glm::toMat4(Grotation);
                                          tc.ApplyTransformToLocal(glm::translate(Matrix4(1.0f), (Vector3&)Gtranslation) * rotMat * glm::scale(Matrix4(1.0f), (Vector3&)Gscale));
                                      },
                       [&](const fastgltf::TRS& trs) {
                           Vector3 Gscale = { trs.scale.x(), trs.scale.y(), trs.scale.z() };
                           Quaternion Grotation = { trs.rotation.w(), trs.rotation.x(), trs.rotation.y(), trs.rotation.z() };
                           Vector3 Gtranslation = { trs.translation.x(), trs.translation.y(), trs.translation.z() };
                           Matrix4 rotMat = glm::toMat4(Grotation);
                           tc.ApplyTransformToLocal(glm::translate(Matrix4(1.0f), (Vector3&)Gtranslation) * rotMat * glm::scale(Matrix4(1.0f), (Vector3&)Gscale));
                       } },
            node.transform);

        SceneEntity.AddChild(ent);
    }
}
}
