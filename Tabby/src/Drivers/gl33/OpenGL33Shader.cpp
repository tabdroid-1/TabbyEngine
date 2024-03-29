#include "Drivers/gl33/OpenGL33Shader.h"
#include "Drivers/gl33/GL33.h"
#include "Tabby/Core/Timer.h"
#include "tbpch.h"

#include <fstream>

#include <glad/gl33.h>
#include <glm/gtc/type_ptr.hpp>
#include <string>

namespace Tabby {

static GLenum ShaderTypeFromString(const std::string& type)
{
    if (type == "vertex")
        return GL_VERTEX_SHADER;
    if (type == "fragment" || type == "pixel")
        return GL_FRAGMENT_SHADER;
    TB_CORE_ASSERT(false, "unknown shader type");
    return 0;
}

OpenGL33Shader::OpenGL33Shader(const std::string& filepath)
{
    TB_PROFILE_SCOPE();

    auto source = ReadFile(filepath);
    auto shaderSources = PreProcess(source);

    Compile(shaderSources, "Path: " + filepath);

    auto lastSlash = filepath.find_last_of("/\\");
    lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
    auto lastDot = filepath.rfind('.');

    auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;

    m_Name = filepath.substr(lastSlash, count);
}

OpenGL33Shader::OpenGL33Shader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource)
    : m_Name(name)
{
    TB_PROFILE_SCOPE();

    std::unordered_map<GLenum, std::string> sources;
    sources[GL_VERTEX_SHADER] = vertexSource;
    sources[GL_FRAGMENT_SHADER] = fragmentSource;
    Compile(sources, "Name: " + name);
}

OpenGL33Shader::~OpenGL33Shader()
{
    TB_PROFILE_SCOPE_NAME("Delete Shader");

    GL33::GL()->DeleteProgram(m_RendererID);
}

std::unordered_map<GLenum, std::string> OpenGL33Shader::PreProcess(const std::string& source)
{
    std::unordered_map<GLenum, std::string> shaderSources;

    const char* typeToken = "#type";
    size_t typeTokenLength = strlen(typeToken);
    size_t pos = source.find(typeToken, 0);
    while (pos != std::string::npos) {
        size_t eol = source.find_first_of("\r\n", pos);
        TB_CORE_ASSERT(eol != std::string::npos, "Syntax error");
        size_t begin = pos + typeTokenLength + 1;
        std::string type = source.substr(begin, eol - begin);

        size_t nextLinePos = source.find_first_not_of("\r\n", eol);
        pos = source.find(typeToken, nextLinePos);
        shaderSources[ShaderTypeFromString(type)] = source.substr(nextLinePos,
            pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
    }
    return shaderSources;
}

std::string OpenGL33Shader::ReadFile(const std::string& filepath)
{
    std::string result;
    std::ifstream in(filepath, std::ios::in | std::ios::binary);

    if (in) {
        in.seekg(0, std::ios::end);
        result.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&result[0], result.size());
    } else {

        TB_CORE_ERROR("Could not open file {0}", filepath);
        TB_CORE_INFO("Current working dir: {0}", std::filesystem::current_path());
    }
    return result;
}

// NOTE: ShaderInfo holds name or path of shader. this is to show path or name for the shader with error
void OpenGL33Shader::Compile(const std::unordered_map<GLenum, std::string>& shaderSource, const std::string& shaderInfo)
{
    TB_PROFILE_SCOPE_NAME("Compile Shader");

    GLuint program = GL33::GL()->CreateProgram();
    TB_CORE_ASSERT(shaderSource.size() <= 2, "Only 3 shaders are supported");
    std::array<GLenum, 3> glShaderIDs;
    int glShaderIDIndex = 0;
    for (auto& kv_pair : shaderSource) {
        GLenum type = kv_pair.first;
        const std::string& sourceBase = kv_pair.second;

        GLuint shader = GL33::GL()->CreateShader(type);

        const GLchar* source = sourceBase.c_str();
        GL33::GL()->ShaderSource(shader, 1, &source, 0);

        // Compile the vertex shader
        GL33::GL()->CompileShader(shader);

        GLint isCompiled = 0;
        GL33::GL()->GetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
        if (isCompiled == GL_FALSE) {
            GLint maxLength = 0;
            GL33::GL()->GetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

            std::vector<GLchar> infoLog(maxLength);
            GL33::GL()->GetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

            GL33::GL()->DeleteShader(shader);

            TB_CORE_ERROR("Shader compilation error for {0}!", shaderInfo);
            TB_CORE_ERROR("{0}", infoLog.data());
            TB_CORE_ASSERT(false, "Shader compilation issue");

            break;
        }
        GL33::GL()->AttachShader(program, shader);
        glShaderIDs[glShaderIDIndex++] = shader;
    }

    GL33::GL()->LinkProgram(program);
    GLint isLinked = 0;
    GL33::GL()->GetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
    if (isLinked != 1) {
        GLint maxLength = 0;
        GL33::GL()->GetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        GL33::GL()->GetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

        // We don't need the program anymore.
        GL33::GL()->DeleteProgram(program);

        for (auto id : glShaderIDs)
            GL33::GL()->DeleteShader(id);

        TB_CORE_ERROR("Linking shader error for {0}!", shaderInfo);
        TB_CORE_ERROR("{0}", infoLog.data());
        TB_CORE_ASSERT(false, "Shader linking issue");
        return;
    }
    for (auto id : glShaderIDs)
        GL33::GL()->DetachShader(program, id);

    m_RendererID = program;
}

void OpenGL33Shader::Bind() const
{
    TB_PROFILE_SCOPE_NAME("Bind");

    GL33::GL()->UseProgram(m_RendererID);
}

void OpenGL33Shader::Unbind() const
{
    TB_PROFILE_SCOPE_NAME("Unbind");

    GL33::GL()->UseProgram(0);
}

void OpenGL33Shader::SetInt(const std::string& name, int value)
{
    TB_PROFILE_SCOPE_NAME("Set int");

    UploadUniformInt(name, value);
}

void OpenGL33Shader::SetIntArray(const std::string& name, int* values, uint32_t count)
{
    TB_PROFILE_SCOPE_NAME("Set int array");

    UploadUniformIntArray(name, values, count);
}

void OpenGL33Shader::SetFloat(const std::string& name, float value)
{
    TB_PROFILE_SCOPE_NAME("Set float");

    UploadUniformFloat(name, value);
}

void OpenGL33Shader::SetFloat2(const std::string& name, const glm::vec2& value)
{
    TB_PROFILE_SCOPE_NAME("Set float 2");

    UploadUniformFloat2(name, value);
}

void OpenGL33Shader::SetFloat3(const std::string& name, const glm::vec3& value)
{
    TB_PROFILE_SCOPE_NAME("Set float 3");

    UploadUniformFloat3(name, value);
}

void OpenGL33Shader::SetFloat4(const std::string& name, const glm::vec4& value)
{
    TB_PROFILE_SCOPE_NAME("Set float 4");

    UploadUniformFloat4(name, value);
}

void OpenGL33Shader::SetMat4(const std::string& name, const glm::mat4& value)
{
    TB_PROFILE_SCOPE_NAME("Set mat 4");

    UploadUniformMat4(name, value);
}

void OpenGL33Shader::UploadUniformInt(const std::string& name, int value)
{
    GLint location = GL33::GL()->GetUniformLocation(m_RendererID, name.c_str());
    GL33::GL()->Uniform1i(location, value);
}

void OpenGL33Shader::UploadUniformIntArray(const std::string& name, int* values, uint32_t count)
{
    GLint location = GL33::GL()->GetUniformLocation(m_RendererID, name.c_str());
    GL33::GL()->Uniform1iv(location, count, values);
}

void OpenGL33Shader::UploadUniformFloat(const std::string& name, float value)
{
    GLint location = GL33::GL()->GetUniformLocation(m_RendererID, name.c_str());
    GL33::GL()->Uniform1f(location, value);
}

void OpenGL33Shader::UploadUniformFloat2(const std::string& name, const glm::vec2& value)
{
    GLint location = GL33::GL()->GetUniformLocation(m_RendererID, name.c_str());
    GL33::GL()->Uniform2f(location, value.x, value.y);
}

void OpenGL33Shader::UploadUniformFloat3(const std::string& name, const glm::vec3& value)
{
    GLint location = GL33::GL()->GetUniformLocation(m_RendererID, name.c_str());
    GL33::GL()->Uniform3f(location, value.x, value.y, value.z);
}

void OpenGL33Shader::UploadUniformFloat4(const std::string& name, const glm::vec4& value)
{
    GLint location = GL33::GL()->GetUniformLocation(m_RendererID, name.c_str());
    GL33::GL()->Uniform4f(location, value.x, value.y, value.z, value.w);
}

void OpenGL33Shader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
{
    GLint location = GL33::GL()->GetUniformLocation(m_RendererID, name.c_str());
    GL33::GL()->UniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void OpenGL33Shader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
{
    GLint location = GL33::GL()->GetUniformLocation(m_RendererID, name.c_str());
    GL33::GL()->UniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

}
