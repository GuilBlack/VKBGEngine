#pragma once
#include "glm/gtc/matrix_transform.hpp"

namespace vkbg
{
struct TransformComponent
{
    glm::vec3 Translation{};
    glm::vec3 Scale{ 1.f, 1.f, 1.f };
    glm::vec3 Rotation{};

    glm::mat4 GetTransform() 
    {
        const float c3 = glm::cos(Rotation.z);
        const float s3 = glm::sin(Rotation.z);
        const float c2 = glm::cos(Rotation.x);
        const float s2 = glm::sin(Rotation.x);
        const float c1 = glm::cos(Rotation.y);
        const float s1 = glm::sin(Rotation.y);
        return glm::mat4{
            {
                Scale.x * (c1 * c3 + s1 * s2 * s3),
                Scale.x * (c2 * s3),
                Scale.x * (c1 * s2 * s3 - c3 * s1),
                0.0f,
            },
            {
                Scale.y * (c3 * s1 * s2 - c1 * s3),
                Scale.y * (c2 * c3),
                Scale.y * (c1 * c3 * s2 + s1 * s3),
                0.0f,
            },
            {
                Scale.z * (c2 * s1),
                Scale.z * (-s2),
                Scale.z * (c1 * c2),
                0.0f,
            },
            {Translation.x, Translation.y, Translation.z, 1.0f}
        };

    }
};

class Entity
{
public:
    using id_t = uint32_t;

    static Entity CreateEntity()
    {
        static id_t nextID = 0;
        return Entity{ nextID };
    }

public:
    std::shared_ptr<class Model> Model;
    glm::vec3 Color{};
    TransformComponent Transform;

public:
    Entity(const Entity&) = delete;
    Entity& operator=(const Entity&) = delete;
    Entity(Entity&&) = default;
    Entity& operator=(Entity&&) = default;

    id_t GetID() { return m_ID; }

private:
    id_t m_ID;

private:
    Entity(id_t id)
        : m_ID{ id } {}
};
}
