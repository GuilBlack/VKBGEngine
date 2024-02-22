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

void Engine::LoadEntities()
{
    std::shared_ptr<Model> vaseModel = Model::CreateModelFromObj(m_RenderContext, "res/Models/smooth_vase.obj");

    Entity vase = Entity::CreateEntity();
    vase.Model = vaseModel;

    vase.Transform.Translation = { 0.f, 0.f, 2.5f };
    vase.Transform.Scale = { .5f, .5f, .5f };

    m_Entities.emplace_back(std::move(vase));
}
}
