#include "KeyboardMovementController.h"
#include "Entities/Entity.h"

namespace vkbg
{
void KeyboardMovementController::MoveInPlaneXZ(GLFWwindow* window, float dt, Entity& entity)
{
    glm::vec3 rotate{ 0 };
    if (glfwGetKey(window, Keys.LookRight) == GLFW_PRESS) 
        rotate.y += 1.f;
    if (glfwGetKey(window, Keys.LookLeft) == GLFW_PRESS) 
        rotate.y -= 1.f;
    if (glfwGetKey(window, Keys.LookUp) == GLFW_PRESS) 
        rotate.x += 1.f;
    if (glfwGetKey(window, Keys.LookDown) == GLFW_PRESS) 
        rotate.x -= 1.f;

    if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon())
    {
        entity.Transform.Rotation += LookSpeed * dt * glm::normalize(rotate);
    }

    // limit pitch values between about +/- 85ish degrees
    float xRot = (float)entity.Transform.Rotation.x;
    float yRot = (float)entity.Transform.Rotation.y;
    entity.Transform.Rotation.x = glm::clamp(xRot, -1.5f, 1.5f);
    entity.Transform.Rotation.y = glm::mod(yRot, glm::two_pi<float>());

    float yaw = entity.Transform.Rotation.y;
    const glm::vec3 forwardDir{ sin(yaw), 0.f, cos(yaw) };
    const glm::vec3 rightDir{ forwardDir.z, 0.f, -forwardDir.x };
    const glm::vec3 upDir{ 0.f, -1.f, 0.f };

    glm::vec3 moveDir{ 0.f };
    if (glfwGetKey(window, Keys.MoveForward) == GLFW_PRESS)
        moveDir += forwardDir;
    if (glfwGetKey(window, Keys.MoveBackward) == GLFW_PRESS)
        moveDir -= forwardDir;
    if (glfwGetKey(window, Keys.MoveRight) == GLFW_PRESS)
        moveDir += rightDir;
    if (glfwGetKey(window, Keys.MoveLeft) == GLFW_PRESS)
        moveDir -= rightDir;
    if (glfwGetKey(window, Keys.MoveUp) == GLFW_PRESS)
        moveDir += upDir;
    if (glfwGetKey(window, Keys.MoveDown) == GLFW_PRESS)
        moveDir -= upDir;

    if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon())
    {
        entity.Transform.Translation += MoveSpeed * dt * glm::normalize(moveDir);
    }
}
}
