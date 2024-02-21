#pragma once
#include "WindowProps.h"
#include "Entities/Entity.h"

namespace vkbg
{
struct EngineProps
{
    WindowProps WindowProperties;
};

class Engine
{
public:
    ~Engine() {}

    static Engine& Instance()
    {
        static Engine instance;
        return instance;
    }

    void Init(EngineProps properties);
    void Run();
    void Shutdown();

private:
    Engine() = default;
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;
    Engine(Engine&&) = delete;
    Engine& operator=(Engine&&) = delete;

    std::unique_ptr<class Model> CreateCubeModel(class RenderContext* context, glm::vec3 offset);

    void LoadEntities();


private:
    class Window* m_Window{ nullptr };
    class RenderContext* m_RenderContext{ nullptr };
    class Renderer* m_Renderer{ nullptr };
    std::vector<Entity> m_Entities;
};
}

