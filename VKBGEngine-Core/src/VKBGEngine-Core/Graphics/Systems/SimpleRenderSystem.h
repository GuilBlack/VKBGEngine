#pragma once
#include "Entities/Entity.h"

namespace vkbg
{
class SimpleRenderSystem
{
public:
    SimpleRenderSystem(class RenderContext* context, VkRenderPass renderPass);
    ~SimpleRenderSystem();

    void RenderEntities(VkCommandBuffer commandBuffer, std::vector<Entity>& entities, const class Camera& camera);

private:
    SimpleRenderSystem(const SimpleRenderSystem&) = delete;
    SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;
    SimpleRenderSystem(SimpleRenderSystem&&) = delete;
    SimpleRenderSystem& operator=(SimpleRenderSystem&&) = delete;

    void CreatePipelineLayout();
    void CreatePipeline(VkRenderPass renderPass);

private:
    // references
    class RenderContext* m_Context;

private:
    class Pipeline* m_Pipeline{ nullptr };
    VkPipelineLayout m_PipelineLayout;

};
}

