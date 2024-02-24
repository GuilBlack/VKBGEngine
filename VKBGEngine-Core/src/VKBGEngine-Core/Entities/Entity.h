#pragma once
#include "glm/gtc/matrix_transform.hpp"

namespace vkbg
{
struct TransformComponent
{
    glm::vec3 Translation{};
    glm::vec3 Scale{ 1.f, 1.f, 1.f };
    glm::vec3 Rotation{};

    glm::mat4 GetTransform();

    glm::mat3 GetNormalMatrix();
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
