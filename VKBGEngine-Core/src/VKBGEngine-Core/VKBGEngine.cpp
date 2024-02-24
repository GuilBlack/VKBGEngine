#include "VKBGEngine.h"
#include "Window.h"
#include "WindowProps.h"
#include "FrameInfo.h"
#include "Graphics/Pipeline.h"
#include "Graphics/RenderContext.h"
#include "Graphics/SwapChain.h"
#include "Graphics/Renderer.h"
#include "Graphics/Model.h"
#include "Graphics/Systems/SimpleRenderSystem.h"
#include "Entities/Camera.h"
#include "Inputs/KeyboardMovementController.h"
#include "Graphics/Buffer.h"
#include "Graphics/Descriptors.h"

namespace vkbg
{
struct GlobalUbo
{
    glm::mat4 projectionView{ 1.f };
    glm::vec3 directionalLightPos = glm::normalize(glm::vec3{ 1.f, -3.f, -1.f });
};

void Engine::Init(EngineProps properties)
{
    glfwInit();

    m_Window = new Window(properties.WindowProperties);

    m_RenderContext = new RenderContext(m_Window);

    m_Renderer = new Renderer(m_Window, m_RenderContext);

    m_GlobalDescriptorPool = DescriptorPool::Builder(m_RenderContext)
        .SetMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
        .AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
        .Build();

    LoadEntities();
}

void Engine::Run()
{
    std::vector<Buffer> globalUbos;
    globalUbos.reserve(SwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < globalUbos.capacity(); ++i)
    {
        globalUbos.emplace_back(std::move(Buffer{
            m_RenderContext,
            sizeof(GlobalUbo),
            1,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
        }));

        globalUbos[i].Map();
    }

    auto globalSetLayout = DescriptorSetLayout::Builder(m_RenderContext)
        .AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
        .Build();

    std::vector<VkDescriptorSet> globalDescriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int32_t i = 0; i < globalDescriptorSets.size(); ++i)
    {
        auto bufferInfo = globalUbos[i].DescriptorInfo();
        DescriptorWriter(*globalSetLayout, *m_GlobalDescriptorPool)
            .WriteBuffer(0, &bufferInfo)
            .Build(globalDescriptorSets[i]);
    }

    SimpleRenderSystem srs{ m_RenderContext, m_Renderer->GetSwapChainRenderPass(), globalSetLayout->GetDescriptorSetLayout() };
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
        uint32_t frameIndex = m_Renderer->GetCurrentFrameIndex();

        // update
        GlobalUbo ubo{
            .projectionView = camera.GetProjectionMatrix() * camera.GetViewMatrix(),
        };
        globalUbos[frameIndex].WriteToBuffer(&ubo);
        globalUbos[frameIndex].Flush();

        FrameInfo fi{ frameIndex, frameTime, commandBuffer, camera, globalDescriptorSets[frameIndex] };

        // render
        m_Renderer->BeginSwapChainRenderPass(commandBuffer);
        srs.RenderEntities(commandBuffer, m_Entities, fi);
        m_Renderer->EndSwapChainRenderPass(commandBuffer);
        m_Renderer->EndFrame();
    }

    vkDeviceWaitIdle(m_RenderContext->GetLogicalDevice());
}

void Engine::Shutdown()
{
    m_Entities.clear();
    
    delete m_GlobalDescriptorPool;
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
    vase.Transform.Scale = { 2.5f, 1.5f, 2.5f };

    m_Entities.emplace_back(std::move(vase));
}
}
