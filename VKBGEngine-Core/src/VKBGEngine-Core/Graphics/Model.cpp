#include "Model.h"
#include "RenderContext.h"

namespace vkbg
{
Model::Model(RenderContext* context, const Model::Builder& builder)
    : m_Context(context)
{
    CreateVertexBuffer(builder.Vertices);
    if (builder.Indices.size() > 0)
    {
        CreateIndexBuffer(builder.Indices);
        m_HasIndexBuffer = true;
    }
}

Model::~Model()
{
    vkDestroyBuffer(m_Context->GetLogicalDevice(), m_VertexBuffer, nullptr);
    vkFreeMemory(m_Context->GetLogicalDevice(), m_VertexBufferMemory, nullptr);

    if (m_HasIndexBuffer)
    {
        vkDestroyBuffer(m_Context->GetLogicalDevice(), m_IndexBuffer, nullptr);
        vkFreeMemory(m_Context->GetLogicalDevice(), m_IndexBufferMemory, nullptr);
    }
}

void Model::Bind(VkCommandBuffer commandBuffer)
{
    VkBuffer buffers[]{ m_VertexBuffer };
    VkDeviceSize offsets[]{ 0 };

    vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

    if (m_HasIndexBuffer)
        vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer, 0, VK_INDEX_TYPE_UINT32);
}

void Model::Draw(VkCommandBuffer commandBuffer)
{
    if (m_HasIndexBuffer)
        vkCmdDrawIndexed(commandBuffer, m_IndexCount, 1, 0, 0, 0);
    else
        vkCmdDraw(commandBuffer, m_VertexCount, 1, 0, 0);
}

void Model::CreateVertexBuffer(const std::vector<Vertex>& vertices)
{
    m_VertexCount = (uint32_t)vertices.size();
    assert(m_VertexCount >= 3 && "vertext count must be at least 3");
    VkDeviceSize bufferSize = sizeof(vertices[0]) * m_VertexCount;

    m_Context->CreateDeviceLocalBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, m_VertexBuffer, m_VertexBufferMemory, vertices.data());
}

void Model::CreateIndexBuffer(const std::vector<uint32_t>& indices)
{
    m_IndexCount = (uint32_t)indices.size();
    VkDeviceSize bufferSize = sizeof(indices[0]) * m_IndexCount;

    m_Context->CreateDeviceLocalBuffer(bufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, m_IndexBuffer, m_IndexBufferMemory, indices.data());
}

std::vector<VkVertexInputBindingDescription> Model::Vertex::GetBindingDescriptions()
{
    std::vector<VkVertexInputBindingDescription> bindingDescriptions{
        {
            .binding = 0,
            .stride = sizeof(Vertex),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
        }
    };
    return bindingDescriptions;
}

std::vector<VkVertexInputAttributeDescription> Model::Vertex::GetAttributeDescriptions()
{
    std::vector<VkVertexInputAttributeDescription> attributeDescription{
        {
            .location = 0,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(Vertex, Position)
        },
        {
            .location = 1,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(Vertex, Color)
        }
    };
    return attributeDescription;
}
}
