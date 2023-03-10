#ifndef RENDERER_HPP
#define RENDERER_HPP

BEGIN_VISUALIZER_NAMESPACE

class Camera;

class Renderer
{
public:
    Renderer(uint32_t width, uint32_t height, const std::shared_ptr<Camera>& camera)
        : m_ViewportWidth(width)
        , m_ViewportHeight(height)
        , m_Camera(camera)
    {}

    Renderer() = delete;
    ~Renderer() = default;
    Renderer(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;

    Renderer& operator=(const Renderer&) = delete;
    Renderer& operator=(Renderer&&) = delete;

    bool Initialize();
    void Render();
    void Cleanup();
    bool CompileShaders();

    void UpdateViewport(uint32_t width, uint32_t height);
    void UpdateCamera();

private:
    struct UBOData {
        glm::mat4 viewProjectionMatrix;
        glm::vec3 cameraPos;
    };

    GLuint m_UBO, m_VBO, m_IBO, m_VAO, m_ShaderProgram;

    uint32_t m_IndexCount;

    UBOData* m_UBOData = nullptr;

    std::shared_ptr<Camera> m_Camera;
    uint32_t m_ViewportWidth, m_ViewportHeight;
};

END_VISUALIZER_NAMESPACE

#endif // !RENDERER_HPP
