#include "VKBGEngine.h"
#include "Window.h"
#include "Graphics/Pipeline.h"
#include "Graphics/RenderContext.h"
#include "Graphics/SwapChain.h"
#include "Graphics/Renderer.h"
#include "Graphics/Model.h"
#include "Graphics/Systems/SimpleRenderSystem.h"
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

    while (!m_Window->ShouldClose())
    {
        glfwPollEvents();
        VkCommandBuffer commandBuffer{};
        if (m_Renderer->BeginFrame(commandBuffer) == false)
            continue;
        m_Renderer->BeginSwapChainRenderPass(commandBuffer);
        srs.RenderEntities(commandBuffer, m_Entities);
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
    std::vector<Model::Vertex> vertices{
        {{ 0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}}
    };
    auto model = std::make_shared<Model>(m_RenderContext, vertices);

    auto triangle = Entity::CreateEntity();
    triangle.Model = model;
    triangle.Color = { .1f, .8f, .1f };
    triangle.Transform2D.Translation.x = .2f;
    triangle.Transform2D.Rotation = glm::radians(45.f);
    triangle.Transform2D.Scale = { 1.5f, 1.f };

    m_Entities.emplace_back(std::move(triangle));
}
}
