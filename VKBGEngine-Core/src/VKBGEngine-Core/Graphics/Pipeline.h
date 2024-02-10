#pragma once

namespace vkbg
{
struct PipelineProps
{
    VkViewport Viewport;
    VkRect2D Scissor;
    VkPipelineViewportStateCreateInfo ViewportInfo;
    VkPipelineInputAssemblyStateCreateInfo InputAssemblyInfo;
    VkPipelineRasterizationStateCreateInfo RasterizationInfo;
    VkPipelineMultisampleStateCreateInfo MultisampleInfo;
    VkPipelineColorBlendAttachmentState ColorBlendAttachment;
    VkPipelineColorBlendStateCreateInfo ColorBlendInfo;
    VkPipelineDepthStencilStateCreateInfo DepthStencilInfo;
    VkPipelineLayout PipelineLayout{ nullptr };
    VkRenderPass RenderPass{ nullptr };
    uint32_t SubPass{ 0 };
};

class Pipeline
{
public:
    Pipeline(
        class RenderContext* context, 
        const std::string& vertShaderPath, 
        const std::string& fragShaderPath, 
        PipelineProps properties);
    ~Pipeline();

    Pipeline(const Pipeline&) = delete;
    void operator=(const Pipeline&) = delete;

public:
    static PipelineProps GetDefaultPipelineProps(uint32_t width, uint32_t height);

private:
    void CreateGraphicsPipeline(
        const std::string& vertShaderPath,
        const std::string& fragShaderPath,
        PipelineProps properties);

private:
    /// <summary>
    /// Read a binary files and return a vector of bytes
    /// </summary>
    static std::vector<uint8_t> ReadFile(const std::string& filePath);
    void CreateShaderModule(const std::vector<uint8_t>& code, VkShaderModule* shaderModule);

private:
    class RenderContext* m_Context;
    VkPipeline m_Pipeline;
    VkShaderModule m_VertexShaderModule;
    VkShaderModule m_FragmentShaderModule;
};

}

