#include "VKBGEngine.h"
#include "Window.h"
#include "Graphics/Pipeline.h"
#include "Graphics/RenderContext.h"
#include "Graphics/SwapChain.h"
#include "Graphics/Renderer.h"
#include "Graphics/Model.h"
#include "Graphics/Systems/SimpleRenderSystem.h"
#include "Entities/Camera.h"
#include "WindowProps.h"

namespace vkbg
{
void Engine::Init(EngineProps properties)
{
    glfwInit();

    m_Window = new Window(properties.WindowProperties);

    m_RenderContext = new RenderContext(m_Window);

    m_Renderer = new Renderer(m_Window, m_RenderContext);

    LoadEntities();
}

void Engine::Run()
{
    SimpleRenderSystem srs{ m_RenderContext, m_Renderer->GetSwapChainRenderPass() };
    Camera camera{};
    camera.SetViewTarget({ -1.f, 1.f , 10.f }, { 0.f, 0.0f, 2.5f });

    while (!m_Window->ShouldClose())
    {
        glfwPollEvents();
        //camera.SetOrthogonalProjection(-1, 1, 1, -1, -1, 1);
        camera.SetPerspectiveProjection(glm::radians(45.f), m_Renderer->GetAspectRatio(), .1f, 100.f);

        VkCommandBuffer commandBuffer{};
        if (m_Renderer->BeginFrame(commandBuffer) == false)
            continue;
        m_Renderer->BeginSwapChainRenderPass(commandBuffer);
        srs.RenderEntities(commandBuffer, m_Entities, camera);
        m_Renderer->EndSwapChainRenderPass(commandBuffer);
        m_Renderer->EndFrame();
    }

    vkDeviceWaitIdle(m_RenderContext->GetLogicalDevice());
}

void Engine::Shutdown()
{
    m_Entities.clear();
    
    delete m_Renderer;
    delete m_RenderContext;
    delete m_Window;
}

std::unique_ptr<Model> Engine::CreateCubeModel(RenderContext* context, glm::vec3 offset)
{
    std::vector<Model::Vertex> vertices{

        // left face (white)
        {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
        {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
        {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
        {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
        {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
        {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},

        // right face (yellow)
        {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
        {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
        {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
        {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
        {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
        {{.5f, .5f, .5f}, {.8f, .8f, .1f}},

        // top face (orange, remember y axis points down)
        {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
        {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
        {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
        {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
        {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
        {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},

        // bottom face (red)
        {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
        {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
        {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
        {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
        {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
        {{.5f, .5f, .5f}, {.8f, .1f, .1f}},

        // nose face (blue)
        {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
        {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
        {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
        {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
        {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
        {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},

        // tail face (green)
        {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
        {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
        {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
        {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
        {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
        {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},

    };
    for (auto& v : vertices)
    {
        v.Position += offset;
    }
    return std::make_unique<Model>(context, vertices);

}

void Engine::LoadEntities()
{
    std::shared_ptr<Model> cubeModel = CreateCubeModel(m_RenderContext, { 0.f, 0.f, 0.f });

    Entity cube = Entity::CreateEntity();
    cube.Model = cubeModel;

    cube.Transform.Translation = { 0.f, 0.f, 2.5f };
    cube.Transform.Scale = { .5f, .5f, .5f };

    m_Entities.emplace_back(std::move(cube));
}
}
