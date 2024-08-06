#include "Drivers/gl46/OpenGL46Shader.h"

#include "tbpch.h"

#include <fstream>

#include <gl.h>
#include <glm/gtc/type_ptr.hpp>
#include <string>

namespace Tabby {

static GLenum ShaderTypeFromString(const std::string& type)
{
    if (type == "vertex")
        return GL_VERTEX_SHADER;
    if (type == "fragment" || type == "pixel")
        return GL_FRAGMENT_SHADER;

    TB_CORE_ASSERT_TAGGED(false, "Unknown shader type!");
    return 0;
}

OpenGL46Shader::OpenGL46Shader(const std::string& filepath)
{
    TB_PROFILE_SCOPE();

    std::string source = ReadFile(filepath);
    auto shaderSources = PreProcess(source);
    Compile(shaderSources, "Path: " + filepath);

    // Extract name from filepath
    auto lastSlash = filepath.find_last_of("/\\");
    lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
    auto lastDot = filepath.rfind('.');
    auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
    m_Name = filepath.substr(lastSlash, count);
}

OpenGL46Shader::OpenGL46Shader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
    : m_Name(name)
{
    TB_PROFILE_SCOPE();

    std::unordered_map<GLenum, std::string> sources;
    sources[GL_VERTEX_SHADER] = vertexSrc;
    sources[GL_FRAGMENT_SHADER] = fragmentSrc;
    Compile(sources, "Name: " + name);
}

OpenGL46Shader::~OpenGL46Shader()
{
    TB_PROFILE_SCOPE();

    glDeleteProgram(m_RendererID);
}

std::string OpenGL46Shader::ReadFile(const std::string& filepath)
{
    TB_PROFILE_SCOPE();

    std::string result;
    std::ifstream in(filepath, std::ios::in | std::ios::binary); // ifstream closes itself due to RAII
    if (in) {
        in.seekg(0, std::ios::end);
        size_t size = in.tellg();
        if (size != -1) {
            result.resize(size);
            in.seekg(0, std::ios::beg);
            in.read(&result[0], size);
        } else {
            TB_CORE_ERROR("Could not read from file '{0}'", filepath);
        }
    } else {
        TB_CORE_ERROR("Could not open file '{0}'", filepath);
    }

    return result;
}

std::unordered_map<GLenum, std::string> OpenGL46Shader::PreProcess(const std::string& source)
{
    TB_PROFILE_SCOPE();

    std::unordered_map<GLenum, std::string> shaderSources;

    const char* typeToken = "#type";
    size_t typeTokenLength = strlen(typeToken);
    size_t pos = source.find(typeToken, 0); // Start of shader type declaration line
    while (pos != std::string::npos) {
        size_t eol = source.find_first_of("\r\n", pos); // End of shader type declaration line
        TB_CORE_ASSERT_TAGGED(eol != std::string::npos, "Syntax error");
        size_t begin = pos + typeTokenLength + 1; // Start of shader type name (after "#type " keyword)
        std::string type = source.substr(begin, eol - begin);
        TB_CORE_ASSERT_TAGGED(ShaderTypeFromString(type), "Invalid shader type specified");

        size_t nextLinePos = source.find_first_not_of("\r\n", eol); // Start of shader code after shader type declaration line
        TB_CORE_ASSERT_TAGGED(nextLinePos != std::string::npos, "Syntax error");
        pos = source.find(typeToken, nextLinePos); // Start of next shader type declaration line

        shaderSources[ShaderTypeFromString(type)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
    }

    return shaderSources;
}

void OpenGL46Shader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources, const std::string& shaderInfo)
{
    TB_PROFILE_SCOPE();

    GLuint program = glCreateProgram();
    TB_CORE_ASSERT_TAGGED(shaderSources.size() <= 2, "We only support 2 shaders for now");
    std::array<GLenum, 2> glShaderIDs;
    int glShaderIDIndex = 0;
    for (auto& kv : shaderSources) {
        GLenum type = kv.first;
        const std::string& source = kv.second;

        GLuint shader = glCreateShader(type);

        const GLchar* sourceCStr = source.c_str();
        glShaderSource(shader, 1, &sourceCStr, 0);

        glCompileShader(shader);

        GLint isCompiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
        if (isCompiled == GL_FALSE) {
            GLint maxLength = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

            std::vector<GLchar> infoLog(maxLength);
            glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

            glDeleteShader(shader);

            TB_CORE_ERROR("Shader compilation error for {0}!", shaderInfo);
            TB_CORE_ERROR("{0}", infoLog.data());
            TB_CORE_ASSERT_TAGGED(false, "Shader compilation failure!");
            break;
        }

        glAttachShader(program, shader);
        glShaderIDs[glShaderIDIndex++] = shader;
    }

    m_RendererID = program;

    // Link our program
    glLinkProgram(program);

    // Note the different functions here: glGetProgram* instead of glGetShader*.
    GLint isLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
    if (isLinked == GL_FALSE) {
        GLint maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

        // We don't need the program anymore.
        glDeleteProgram(program);

        for (auto id : glShaderIDs)
            glDeleteShader(id);

        TB_CORE_ERROR("Linking shader error for {0}!", shaderInfo);
        TB_CORE_ERROR("{0}", infoLog.data());
        TB_CORE_ASSERT_TAGGED(false, "Shader link failure!");
        return;
    }

    for (auto id : glShaderIDs) {
        glDetachShader(program, id);
        glDeleteShader(id);
    }
}

void OpenGL46Shader::Bind() const
{
    TB_PROFILE_SCOPE();

    glUseProgram(m_RendererID);
}

void OpenGL46Shader::Unbind() const
{
    TB_PROFILE_SCOPE();

    glUseProgram(0);
}

void OpenGL46Shader::SetBool(const std::string& name, bool value)
{
    TB_PROFILE_SCOPE_NAME("Set bool");

    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniform1i(location, (int)value);
}

void OpenGL46Shader::SetInt(const std::string& name, int value)
{
    TB_PROFILE_SCOPE_NAME("Set int");

    UploadUniformInt(name, value);
}

void OpenGL46Shader::SetIntArray(const std::string& name, int* values, uint32_t count)
{
    TB_PROFILE_SCOPE_NAME("Set int array");

    UploadUniformIntArray(name, values, count);
}

void OpenGL46Shader::SetFloatArray(const std::string& name, float* values, uint32_t count)
{
    TB_PROFILE_SCOPE_NAME("Set int array");

    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniform1fv(location, count, values);
}

void OpenGL46Shader::SetFloat(const std::string& name, float value)
{
    TB_PROFILE_SCOPE_NAME("Set float");

    UploadUniformFloat(name, value);
}

void OpenGL46Shader::SetFloat2(const std::string& name, const glm::vec2& value)
{
    TB_PROFILE_SCOPE_NAME("Set float 2");

    UploadUniformFloat2(name, value);
}

void OpenGL46Shader::SetFloat3(const std::string& name, const glm::vec3& value)
{
    TB_PROFILE_SCOPE_NAME("Set float 3");

    UploadUniformFloat3(name, value);
}

void OpenGL46Shader::SetFloat4(const std::string& name, const glm::vec4& value)
{
    TB_PROFILE_SCOPE_NAME("Set float 4");

    UploadUniformFloat4(name, value);
}

void OpenGL46Shader::SetMat4(const std::string& name, const Matrix4& value)
{
    TB_PROFILE_SCOPE_NAME("Set mat 4");

    UploadUniformMat4(name, value);
}

void OpenGL46Shader::UploadUniformInt(const std::string& name, int value)
{
    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniform1i(location, value);
}

void OpenGL46Shader::UploadUniformIntArray(const std::string& name, int* values, uint32_t count)
{
    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniform1iv(location, count, values);
}

void OpenGL46Shader::UploadUniformFloat(const std::string& name, float value)
{
    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniform1f(location, value);
}

void OpenGL46Shader::UploadUniformFloat2(const std::string& name, const glm::vec2& value)
{
    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniform2f(location, value.x, value.y);
}

void OpenGL46Shader::UploadUniformFloat3(const std::string& name, const glm::vec3& value)
{
    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniform3f(location, value.x, value.y, value.z);
}

void OpenGL46Shader::UploadUniformFloat4(const std::string& name, const glm::vec4& value)
{
    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniform4f(location, value.x, value.y, value.z, value.w);
}

void OpenGL46Shader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
{
    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void OpenGL46Shader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
{
    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

}
