#pragma once

namespace vkbg
{
struct PipelineProps
{
    PipelineProps() = default;
    ~PipelineProps() = default;
    PipelineProps(const PipelineProps&) = delete;
    PipelineProps& operator=(const PipelineProps&) = delete;

    VkPipelineInputAssemblyStateCreateInfo InputAssemblyInfo;
    VkPipelineRasterizationStateCreateInfo RasterizationInfo;
    VkPipelineMultisampleStateCreateInfo MultisampleInfo;
    VkPipelineColorBlendAttachmentState ColorBlendAttachment;
    VkPipelineDepthStencilStateCreateInfo DepthStencilInfo;
    std::vector<VkDynamicState> DynamicStates;
    VkPipelineDynamicStateCreateInfo DynamicStatesInfo;
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
        const PipelineProps& properties);
    ~Pipeline();

    Pipeline(const Pipeline&) = delete;
    Pipeline& operator=(const Pipeline&) = delete;

    void BindToCommandBuffer(VkCommandBuffer commandBuffer);

public:
    static void GetDefaultPipelineProps(PipelineProps& properties);

private:
    void CreateGraphicsPipeline(
        const std::string& vertShaderPath,
        const std::string& fragShaderPath,
        const PipelineProps& properties);

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

