#pragma once

namespace vkbg
{
struct Transform2DComponent
{
    glm::vec2 Translation{};
    glm::vec2 Scale{ 1.f, 1.f };
    float Rotation{ 0 };

    glm::mat2 GetTransform() 
    {
        glm::mat2 scaleMat{
            {Scale.x, 0.f},
            {0.f, Scale.y}
        };

        float c = glm::cos(Rotation);
        float s = glm::sin(Rotation);

        glm::mat2 rotationMat{
            {c, s},
            {-s, c}
        };

        return rotationMat * scaleMat;
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
    Transform2DComponent Transform2D;

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
