#include "VKBGEngine.h"
#include "Window.h"
#include "WindowProps.h"
#include "Graphics/Pipeline.h"
#include "Graphics/RenderContext.h"
#include "Graphics/SwapChain.h"
#include "Graphics/Renderer.h"
#include "Graphics/Model.h"
#include "Graphics/Systems/SimpleRenderSystem.h"
#include "Entities/Camera.h"
#include "Inputs/KeyboardMovementController.h"

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
    auto viewerEntity = Entity::CreateEntity();
    KeyboardMovementController cameraController{};
    auto currentTime = std::chrono::high_resolution_clock::now();

    while (!m_Window->ShouldClose())
    {
        glfwPollEvents();
        auto newTime = std::chrono::high_resolution_clock::now();
        float frameTime =
            std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
        currentTime = newTime;

        cameraController.MoveInPlaneXZ(m_Window->GetWindowHandle(), frameTime, viewerEntity);
        camera.SetViewYXZ(viewerEntity.Transform.Translation, viewerEntity.Transform.Rotation);

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
    Model::Builder builder{};
    builder.Vertices = {
        // left face (white)
        {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
        {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
        {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
        {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},

        // right face (yellow)
        {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
        {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
        {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
        {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},

        // top face (orange, remember y axis points down)
        {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
        {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
        {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
        {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},

        // bottom face (red)
        {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
        {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
        {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
        {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},

        // nose face (blue)
        {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
        {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
        {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
        {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},

        // tail face (green)
        {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
        {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
        {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
        {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
    };

    for (auto& v : builder.Vertices)
    {
        v.Position += offset;
    }

    builder.Indices = { 0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,  8,  9,  10, 8,  11, 9,
        12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21 };

    return std::make_unique<Model>(context, builder);
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
