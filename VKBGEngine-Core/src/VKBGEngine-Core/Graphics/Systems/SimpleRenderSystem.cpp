#include "SimpleRenderSystem.h"
#include "Graphics/Renderer.h"
#include "Graphics/Pipeline.h"
#include "Graphics/RenderContext.h"
#include "Graphics/Model.h"

namespace vkbg
{
struct SimplePushConstantData
{
    glm::mat2 Transform;
    glm::vec2 Offset;
    alignas(16) glm::vec3 Color;
};

SimpleRenderSystem::SimpleRenderSystem(class RenderContext* context, VkRenderPass renderPass)
    : m_Context{context}
{
    CreatePipelineLayout();
    CreatePipeline(renderPass);
}

SimpleRenderSystem::~SimpleRenderSystem()
{
    vkDestroyPipelineLayout(m_Context->GetLogicalDevice(), m_PipelineLayout, nullptr);
    delete m_Pipeline;
}

void SimpleRenderSystem::RenderEntities(VkCommandBuffer commandBuffer, std::vector<Entity>& entities)
{
    m_Pipeline->BindToCommandBuffer(commandBuffer);

    for (auto& entity : entities)
    {
        SimplePushConstantData push{
            .Transform = entity.Transform2D.GetTransform(),
            .Offset = entity.Transform2D.Translation,
            .Color = entity.Color
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

void SimpleRenderSystem::CreatePipelineLayout()
{
    VkPushConstantRange pcRange{
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
        .offset = 0,
        .size = sizeof(SimplePushConstantData)
    };

    VkPipelineLayoutCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 0,
        .pSetLayouts = nullptr,
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
