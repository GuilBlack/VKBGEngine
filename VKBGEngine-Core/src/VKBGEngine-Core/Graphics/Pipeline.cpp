#include "Pipeline.h"
#include "RenderContext.h"
#include "Model.h"

namespace vkbg
{

Pipeline::Pipeline(
    class RenderContext* context,
    const std::string& vertShaderPath,
    const std::string& fragShaderPath,
    const PipelineProps& properties)
    : m_Context{context}
{ 
    CreateGraphicsPipeline(vertShaderPath, fragShaderPath, properties);
}

Pipeline::~Pipeline()
{
    vkDestroyPipeline(m_Context->GetLogicalDevice(), m_Pipeline, nullptr);
    vkDestroyShaderModule(m_Context->GetLogicalDevice(), m_VertexShaderModule, nullptr);
    vkDestroyShaderModule(m_Context->GetLogicalDevice(), m_FragmentShaderModule, nullptr);
}

void Pipeline::BindToCommandBuffer(VkCommandBuffer commandBuffer)
{
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);
}

void Pipeline::GetDefaultPipelineProps(PipelineProps& properties)
{
    properties.InputAssemblyInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE
    };

    properties.RasterizationInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = VK_CULL_MODE_NONE,
        .frontFace = VK_FRONT_FACE_CLOCKWISE,
        .depthBiasEnable = VK_FALSE,
        .lineWidth = 1.f,
    };

    properties.MultisampleInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable = VK_FALSE,
        .minSampleShading = 1.0f
    };

    properties.ColorBlendAttachment = {
        .blendEnable = VK_FALSE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,   // Optional
        .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,  // Optional
        .colorBlendOp = VK_BLEND_OP_ADD,              // Optional
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,   // Optional
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,  // Optional
        .alphaBlendOp = VK_BLEND_OP_ADD,              // Optional
        .colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
      VK_COLOR_COMPONENT_A_BIT
    };

    properties.DepthStencilInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .depthTestEnable = VK_TRUE,
        .depthWriteEnable = VK_TRUE,
        .depthCompareOp = VK_COMPARE_OP_LESS,
        .depthBoundsTestEnable = VK_FALSE,
        .stencilTestEnable = VK_FALSE,
        .front = {},
        .back = {},
        .minDepthBounds = 0.0f,
        .maxDepthBounds = 1.0f,
    };

    properties.DynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    properties.DynamicStatesInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = (uint32_t)properties.DynamicStates.size(),
        .pDynamicStates = properties.DynamicStates.data()
    };
}

void Pipeline::CreateGraphicsPipeline(
    const std::string & vertShaderPath,
    const std::string & fragShaderPath,
    const PipelineProps& properties)
{
    auto vertShaderCode = ReadFile(vertShaderPath);
    LOG("vertShaderCode size: " << vertShaderCode.size() << std::endl);
    auto fragShaderCode = ReadFile(fragShaderPath);
    LOG("fragShaderCode size: " << fragShaderCode.size() << std::endl);

    CreateShaderModule(vertShaderCode, &m_VertexShaderModule);
    CreateShaderModule(fragShaderCode, &m_FragmentShaderModule);

    VkPipelineShaderStageCreateInfo shaderStages[2]{
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .module = m_VertexShaderModule,
            .pName = "main",
            .pSpecializationInfo = nullptr
        },
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module = m_FragmentShaderModule,
            .pName = "main",
            .pSpecializationInfo = nullptr
        }
    };

    auto bindingDescriptions = Model::Vertex::GetBindingDescriptions();
    auto attributeDescriptions = Model::Vertex::GetAttributeDescriptions();

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = (uint32_t)bindingDescriptions.size(),
        .pVertexBindingDescriptions = bindingDescriptions.data(),
        .vertexAttributeDescriptionCount = (uint32_t)attributeDescriptions.size(),
        .pVertexAttributeDescriptions = attributeDescriptions.data()
    };

    VkPipelineViewportStateCreateInfo viewportInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .scissorCount = 1,
    };

    VkPipelineColorBlendStateCreateInfo ColorBlendInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments = &properties.ColorBlendAttachment,
        .blendConstants = { 0.f, 0.f, 0.f, 0.f }
    };
    
    VkGraphicsPipelineCreateInfo pipelineInfo{
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = 2,
        .pStages = shaderStages,
        .pVertexInputState = &vertexInputInfo,
        .pInputAssemblyState = &properties.InputAssemblyInfo,
        .pViewportState = &viewportInfo,
        .pRasterizationState = &properties.RasterizationInfo,
        .pMultisampleState = &properties.MultisampleInfo,
        .pDepthStencilState = &properties.DepthStencilInfo,
        .pColorBlendState = &ColorBlendInfo,
        .pDynamicState = &properties.DynamicStatesInfo,
        .layout = properties.PipelineLayout,
        .renderPass = properties.RenderPass,
        .subpass = properties.SubPass,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1
    };
    
    if (vkCreateGraphicsPipelines(
        m_Context->GetLogicalDevice(), VK_NULL_HANDLE
        , 1, &pipelineInfo
        , nullptr, &m_Pipeline) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create the graphics pipeline");
    }
}

std::vector<uint8_t> Pipeline::ReadFile(const std::string & filePath)
{
    // ate: start reading at the end of the file
    std::ifstream file(filePath, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<uint8_t> buffer(fileSize);

    file.seekg(0);
    file.read((char*)buffer.data(), fileSize);
    file.close();

    return buffer;
}

void Pipeline::CreateShaderModule(const std::vector<uint8_t>& code, VkShaderModule* shaderModule)
{
    VkShaderModuleCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = (uint32_t)code.size(),
        .pCode = reinterpret_cast<const uint32_t*>(code.data())
    };

    if (vkCreateShaderModule(m_Context->GetLogicalDevice(), &createInfo, nullptr, shaderModule) != VK_SUCCESS)
        throw std::runtime_error("Failed to create shader module");
}

}
