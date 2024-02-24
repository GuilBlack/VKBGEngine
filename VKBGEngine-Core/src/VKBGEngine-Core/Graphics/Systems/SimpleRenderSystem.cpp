#include "SimpleRenderSystem.h"
#include "Graphics/Renderer.h"
#include "Graphics/Pipeline.h"
#include "Graphics/RenderContext.h"
#include "Graphics/Model.h"
#include "Entities/Camera.h"
#include "FrameInfo.h"

namespace vkbg
{
struct SimplePushConstantData
{
    glm::mat4 ModelMatrix{ 1.f };
    glm::mat4 NormalMatrix{ 1.f };
};

SimpleRenderSystem::SimpleRenderSystem(
    class RenderContext* context,
    VkRenderPass renderPass,
    VkDescriptorSetLayout globalSetLayout)
    : m_Context{context}
{
    CreatePipelineLayout(globalSetLayout);
    CreatePipeline(renderPass);
}

SimpleRenderSystem::~SimpleRenderSystem()
{
    vkDestroyPipelineLayout(m_Context->GetLogicalDevice(), m_PipelineLayout, nullptr);
    delete m_Pipeline;
}

void SimpleRenderSystem::RenderEntities(VkCommandBuffer commandBuffer, std::vector<Entity>& entities, const FrameInfo& frameInfo)
{
    m_Pipeline->BindToCommandBuffer(commandBuffer);

    vkCmdBindDescriptorSets(
        frameInfo.CommandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        m_PipelineLayout,
        0, 1, &frameInfo.GlobalDescriptorSet,
        0, nullptr
    );

    for (auto& entity : entities)
    {
        SimplePushConstantData push{
            .ModelMatrix = entity.Transform.GetTransform(),
            .NormalMatrix = entity.Transform.GetNormalMatrix()
        };

        vkCmdPushConstants(
            commandBuffer,
            m_PipelineLayout,
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            0,
            sizeof(SimplePushConstantData),
            &push
        );

        entity.Model->Bind(commandBuffer);
        entity.Model->Draw(commandBuffer);
    }
}

void SimpleRenderSystem::CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout)
{
    VkPushConstantRange pcRange{
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
        .offset = 0,
        .size = sizeof(SimplePushConstantData)
    };

    std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ globalSetLayout };

    VkPipelineLayoutCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = (uint32_t)descriptorSetLayouts.size(),
        .pSetLayouts = descriptorSetLayouts.data(),
        .pushConstantRangeCount = 1,
        .pPushConstantRanges = &pcRange
    };

    if (vkCreatePipelineLayout(m_Context->GetLogicalDevice(), &createInfo, nullptr, &m_PipelineLayout))
        throw std::runtime_error("Failed to create PipelineLayout");
}

void SimpleRenderSystem::CreatePipeline(VkRenderPass renderPass)
{
    assert(m_PipelineLayout != nullptr && "SwapChain wasn't initialized");

    PipelineProps pipelineProperties{};
    Pipeline::GetDefaultPipelineProps(pipelineProperties);

    pipelineProperties.RenderPass = renderPass;
    pipelineProperties.PipelineLayout = m_PipelineLayout;

    delete m_Pipeline;
    m_Pipeline = new Pipeline(
        m_Context,
        "res/Shaders/Compiled/Simple.vert.spv",
        "res/Shaders/Compiled/Simple.frag.spv",
        pipelineProperties
    );
}
}
