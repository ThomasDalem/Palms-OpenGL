#include <GL/glew.h>

#pragma warning(push, 0)
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>
#pragma warning(pop, 0)

#include <glutils.hpp>
#include <camera.hpp>
#include <renderer.hpp>

#include <iostream>
#include <fstream>
#include <stdexcept>

#include "OBJLoader.hpp"
#include "Vertex.hpp"

BEGIN_VISUALIZER_NAMESPACE

std::string readFile(const std::string &filename)
{
    std::ifstream file(filename);

    if (file.is_open() == false) {
        std::cout << "Error: could not open file " << filename << std::endl;
    }
    std::string content = "";
    std::string line = "";

    while (!file.eof()) {
        std::getline(file, line);
        content.append(line + "\n");
    }
    return content;
}

bool Renderer::CompileShaders()
{
    GLuint vShader = GL_CALL(glCreateShader, GL_VERTEX_SHADER);
    GLuint fShader = GL_CALL(glCreateShader, GL_FRAGMENT_SHADER);

    m_ShaderProgram = glCreateProgram();

    GL_CALL(glAttachShader, m_ShaderProgram, vShader);
    GL_CALL(glAttachShader, m_ShaderProgram, fShader);

    {
        std::string vertexShader = readFile("../../src/vertexShader.glsl");
        const char *verShaderStr = vertexShader.c_str();
        GL_CALL(glShaderSource, vShader, 1, &verShaderStr, nullptr);

        GL_CALL(glCompileShader, vShader);

        {
            GLint length = 0;

            GL_CALL(glGetShaderiv, vShader, GL_INFO_LOG_LENGTH, &length);

            if (length > 1)
            {
                std::string log(length, '\0');

                GL_CALL(glGetShaderInfoLog, vShader, length, nullptr, log.data());

                std::cerr << "Vertex shader log:\n" << log << '\n';
                return false;
            }
        }

        std::string fragmentShader = readFile("../../src/fragmentShader.glsl");
        const char* fragmentShaderStr = fragmentShader.c_str();
        GL_CALL(glShaderSource, fShader, 1, &fragmentShaderStr, nullptr);

        GL_CALL(glCompileShader, fShader);

        {
            GLint length = 0;

            GL_CALL(glGetShaderiv, fShader, GL_INFO_LOG_LENGTH, &length);

            if (length > 1)
            {
                std::string log(length, '\0');

                GL_CALL(glGetShaderInfoLog, fShader, length, nullptr, log.data());

                std::cerr << "Vertex shader log:\n" << log << '\n';
                return false;
            }
        }
    }

    GL_CALL(glLinkProgram, m_ShaderProgram);

    {
        GLint length = 0;

        GL_CALL(glGetProgramiv, m_ShaderProgram, GL_INFO_LOG_LENGTH, &length);

        if (length > 1)
        {
            std::string log(length, '\0');

            GL_CALL(glGetProgramInfoLog, m_ShaderProgram, length, nullptr, log.data());

            std::cerr << "Shader program log:\n" << log << '\n';
            return false;
        }
    }

    GL_CALL(glDetachShader, m_ShaderProgram, vShader);
    GL_CALL(glDetachShader, m_ShaderProgram, fShader);

    GL_CALL(glDeleteShader, vShader);
    GL_CALL(glDeleteShader, fShader);
    return true;
}

bool Renderer::Initialize()
{
    std::vector<Vertex> vertices;
    std::vector<glm::vec3> normals;
    //std::vector<glm::vec2> textureCoordinates;
    std::vector<uint32_t> indices;

    if (loadOBJ("../../res/palm.obj", vertices, indices) == false) {
        return false;
    }

    GL_CALL(glCreateBuffers, 1, &m_UBO);
    UBOData uboData;
    uboData.viewProjectionMatrix = m_Camera->GetViewProjectionMatrix();
    uboData.cameraPos = m_Camera->GetPosition();
    GL_CALL(glNamedBufferStorage, m_UBO, sizeof(UBOData), &uboData, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT);

    GL_CALL(glCreateBuffers, 1, &m_VBO);
    GL_CALL(glNamedBufferStorage, m_VBO, sizeof(Vertex) * vertices.size(), vertices.data(), 0);

    GL_CALL(glCreateBuffers, 1, &m_IBO);
    GL_CALL(glNamedBufferStorage, m_IBO, sizeof(uint32_t) * indices.size(), indices.data(), 0);
    m_IndexCount = uint32_t(indices.size());

    GL_CALL(glCreateVertexArrays, 1, &m_VAO);
    GL_CALL(glBindVertexArray, m_VAO);
    GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, m_VBO);
    GL_CALL(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, m_IBO);

    GL_CALL(glEnableVertexAttribArray, 0);
    GL_CALL(glVertexAttribPointer, 0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
    GL_CALL(glEnableVertexAttribArray, 1);
    GL_CALL(glVertexAttribPointer, 1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(sizeof(glm::vec3)));
    GL_CALL(glEnableVertexAttribArray, 2);
    GL_CALL(glVertexAttribPointer, 2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(sizeof(glm::vec3) * 2));
    if (CompileShaders() == false) {
        return false;
    }

    glBindVertexArray(0);

    GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, 0);
    GL_CALL(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, 0);

    GL_CALL(glDisableVertexAttribArray, 0);
    GL_CALL(glDisableVertexAttribArray, 1);

    m_UBOData = reinterpret_cast<UBOData*>(glMapNamedBufferRange(m_UBO, 0, sizeof(UBOData), GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT));

    return true;
}

void Renderer::Render()
{
    GL_CALL(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GL_CALL(glUseProgram, m_ShaderProgram);

    GL_CALL(glBindBufferBase, GL_UNIFORM_BUFFER, 0, m_UBO);
    GL_CALL(glBindVertexArray, m_VAO);
    GLint size = 0;
    GL_CALL(glGetBufferParameteriv, GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    GL_CALL(glDrawElements, GL_TRIANGLES, m_IndexCount, GL_UNSIGNED_INT, nullptr);
    GL_CALL(glBindVertexArray, 0);
    GL_CALL(glBindBufferBase, GL_UNIFORM_BUFFER, 0, 0);

    GL_CALL(glUseProgram, 0);
}

void Renderer::Cleanup()
{
    m_UBOData = nullptr;

    GL_CALL(glUnmapNamedBuffer, m_UBO);

    GL_CALL(glDeleteBuffers, 1, &m_VBO);
    GL_CALL(glDeleteBuffers, 1, &m_IBO);
    GL_CALL(glDeleteBuffers, 1, &m_UBO);

    GL_CALL(glDeleteVertexArrays, 1, &m_VAO);

    GL_CALL(glDeleteProgram, m_ShaderProgram);

}

void Renderer::UpdateViewport(uint32_t width, uint32_t height)
{
    m_ViewportWidth = width;
    m_ViewportHeight = height;

    glViewport(0, 0, m_ViewportWidth, m_ViewportHeight);
    m_Camera->ComputeProjection(m_ViewportWidth, m_ViewportHeight);

    m_UBOData->viewProjectionMatrix = m_Camera->GetViewProjectionMatrix();
    m_UBOData->cameraPos = m_Camera->GetPosition();
    GL_CALL(glFlushMappedNamedBufferRange, m_UBO, 0, sizeof(UBOData));
}

void Renderer::UpdateCamera()
{
    m_UBOData->viewProjectionMatrix = m_Camera->GetViewProjectionMatrix();
    m_UBOData->cameraPos = m_Camera->GetPosition();
    GL_CALL(glFlushMappedNamedBufferRange, m_UBO, 0, sizeof(UBOData));
}

END_VISUALIZER_NAMESPACE
