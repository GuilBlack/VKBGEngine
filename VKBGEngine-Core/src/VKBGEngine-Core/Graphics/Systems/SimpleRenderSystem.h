#pragma once
#include "Entities/Entity.h"

namespace vkbg
{
class SimpleRenderSystem
{
public:
    SimpleRenderSystem(
        class RenderContext* context,
        VkRenderPass renderPass,
        VkDescriptorSetLayout globalSetLayout);
    ~SimpleRenderSystem();
    void RenderEntities(VkCommandBuffer commandBuffer, std::vector<Entity>& entities, const struct FrameInfo& frameInfo);

private:
    SimpleRenderSystem(const SimpleRenderSystem&) = delete;
    SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;
    SimpleRenderSystem(SimpleRenderSystem&&) = delete;
    SimpleRenderSystem& operator=(SimpleRenderSystem&&) = delete;

    void CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout);
    void CreatePipeline(VkRenderPass renderPass);

private:
    // references
    class RenderContext* m_Context;

private:
    class Pipeline* m_Pipeline{ nullptr };
    VkPipelineLayout m_PipelineLayout;

};
}

