#include <Tabby/Renderer/Mesh.h>
// #include "Application.h"
#include <Tabby/Renderer/RenderCommand.h>

#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Tabby {

Mesh::Mesh()
    : m_StatsRegistered(false)
{
    m_PrimitiveType = PrimitiveType::Triangles;

    m_Transform = Matrix4(1.0f);
    m_Transform = glm::translate(m_Transform, glm::vec3(0.0f, 0.0f, 0.0f));
    m_Transform = glm::scale(m_Transform, glm::vec3(1.0f, 1.0f, 1.0f));
    // m_Material = CreateShared<Material>("UnlitMaterial", "shaders/gl33/Renderer3D_MeshUnlit.glsl");
}

Mesh::~Mesh()
{
    m_Vertices.clear();
    m_Indices.clear();
    m_VertexArray = nullptr;
    m_StatsRegistered = false;
    m_DrawMode = DrawMode::TRIANGLE;
    m_PrimitiveType = PrimitiveType::Triangles;
}

void Mesh::SetName(const std::string& name)
{
    m_Name = name;
}

void Mesh::SetTransform(const Matrix4& transform)
{
    m_Transform = transform;
}

void Mesh::SetPrimitiveType(PrimitiveType primitiveType)
{
    m_PrimitiveType = primitiveType;
}

void Mesh::SetMaterial(Shared<Material> material)
{
    m_Material = material;
}

void Mesh::SetVertices(std::vector<Vertex> vertices)
{
    m_Vertices = vertices;
}

void Mesh::SetIndices(std::vector<uint32_t> indices)
{
    m_Indices = indices;
}

void Mesh::SetVertex(Vertex vertices)
{
    // TODO:
}

void Mesh::SetIndex(uint32_t indices)
{
    // TODO:
}

void Mesh::AddVertex(Vertex vertex)
{
    m_Vertices.push_back(vertex);
    // auto buffers = m_VertexArray->GetVertexBuffers();
    // buffers[0]->SetSubData((float*)&vertex, sizeof(vertex), (m_Vertices.size() - 1) * sizeof(Vertex));
}

void Mesh::AddIndex(uint32_t index)
{
    // TODO:
    m_Indices.push_back(index);
}

void Mesh::Create(DrawMode drawMode /* = DrawMode::TRIANGLE */)
{
    m_DrawMode = drawMode;

    m_VertexArray = VertexArray::Create();

    Shared<VertexBuffer> vertexBuffer = VertexBuffer::Create((float*)&m_Vertices[0], (uint32_t)m_Vertices.size() * sizeof(Vertex));

    vertexBuffer->SetLayout({ { ShaderDataType::Float4, "a_Position" },
        { ShaderDataType::Float2, "a_TexCoord" },
        { ShaderDataType::Float3, "a_Normal" } });

    m_VertexArray->AddVertexBuffer(vertexBuffer);

    if (m_DrawMode == DrawMode::TRIANGLE) {
        Shared<IndexBuffer> indexBuffer = IndexBuffer::Create(&m_Indices[0], (uint32_t)m_Indices.size());
        m_VertexArray->SetIndexBuffer(indexBuffer);
    }

    m_VertexArray->Unbind();
}

void Mesh::Render()
{
    // To Fragment Shader
    if (m_Material) {
        m_Material->SetMatrix(m_Transform);
        m_Material->Render();
    } else
        TB_CORE_WARN("Mesh named \'{0}\' does not have an material!", GetName());

    if (m_DrawMode == DrawMode::TRIANGLE)
        RenderCommand::DrawIndexed(m_VertexArray, (uint32_t)m_Indices.size());
    else if (m_DrawMode == DrawMode::LINE)
        RenderCommand::DrawLines(m_VertexArray, (uint32_t)m_Vertices.size());

#pragma region STATS
        // AddDrawCalls(1);
        // AddVertices((uint32_t)m_Vertices.size());
        // AddIndices((uint32_t)m_Indices.size());
#pragma endregion
}

void Mesh::Destroy()
{
    for (auto& vertexBuffer : m_VertexArray->GetVertexBuffers()) {
        vertexBuffer->Unbind();
        vertexBuffer->~VertexBuffer();
    }

    auto& indexBuffer = m_VertexArray->GetIndexBuffer();

    indexBuffer->Unbind();
    indexBuffer->~IndexBuffer();

    m_VertexArray->~VertexArray();

    m_Vertices.clear();
    m_Indices.clear();
    m_StatsRegistered = false;
}

void Mesh::CloneMesh(Shared<Mesh> mesh)
{
    this->m_Name = mesh->m_Name;
    this->m_Vertices = mesh->GetVertices();
    this->m_Indices = mesh->GetIndices();
    this->m_PrimitiveType = mesh->m_PrimitiveType;
    this->m_DrawMode = mesh->m_DrawMode;

    Create(this->m_DrawMode);

    this->m_Material->CloneMaterialProperties(mesh->GetMaterial());
}

std::vector<Mesh::Vertex> Mesh::GetWorldSpaceVertices(Vector3 position = Vector3(0, 0, 0), Vector3 eulerAngles = Vector3(0, 0, 0), Vector3 scale = Vector3(1, 1, 1))
{
    // Matrix4 modelMatrix = glm::translate(Matrix4(1), position) * glm::rotate(Matrix4(1), glm::radians(eulerAngles.x), Vector3(1, 0, 0)) * glm::rotate(Matrix4(1), glm::radians(-eulerAngles.y), Vector3(0, 1, 0)) * glm::rotate(Matrix4(1), glm::radians(eulerAngles.z), Vector3(0, 0, 1)) * glm::scale(Matrix4(1), scale);

    Matrix4 rotation = glm::toMat4(glm::quat(glm::radians((glm::vec3)eulerAngles)));
    Matrix4 modelMatrix = glm::translate(Matrix4(1.0f), (glm::vec3)position) * rotation * glm::scale(Matrix4(1.0f), (glm::vec3)scale);

    std::vector<Vertex> worldSpaceVertices;

    for (const Vertex& vertex : m_Vertices) {
        Vertex transformedVertex = Vertex();
        transformedVertex.Position = modelMatrix * vertex.Position;
        // transformedVertex.normal = vertex.normal;
        transformedVertex.TexCoords = vertex.TexCoords;

        worldSpaceVertices.push_back(transformedVertex);
    }

    return worldSpaceVertices;
}
}
