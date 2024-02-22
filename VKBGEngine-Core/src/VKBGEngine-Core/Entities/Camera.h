#pragma once

namespace vkbg
{
class Camera
{
public:
    Camera() = default;
    ~Camera() = default;

    void SetPerspectiveProjection(float fovy, float aspect, float zNear, float zFar)
    {
        assert(glm::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f);
        const float tanHalfFovy = tan(fovy / 2.f);
        m_ProjectionMatrix = glm::mat4{ 0.0f };
        m_ProjectionMatrix[0][0] = 1.f / (aspect * tanHalfFovy);
        m_ProjectionMatrix[1][1] = 1.f / (tanHalfFovy);
        m_ProjectionMatrix[2][2] = zFar / (zFar - zNear);
        m_ProjectionMatrix[2][3] = 1.f;
        m_ProjectionMatrix[3][2] = -(zFar * zNear) / (zFar - zNear);
    }

    void SetOrthogonalProjection(float left, float right, float bottom, float top, float zNear, float zFar)
    {
        m_ProjectionMatrix = glm::orthoRH(left, right, top, bottom, zNear, zFar);
    }

    void SetViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up = {0.0f, -1.0f, 0.0f});
    void SetViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up = {0.0f, -1.0f, 0.0f});
    void SetViewYXZ(glm::vec3 position, glm::vec3 rotation);

    const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
    const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }

private:
    glm::mat4 m_ProjectionMatrix{ 1.f };
    glm::mat4 m_ViewMatrix{ 1.f };
};
}
