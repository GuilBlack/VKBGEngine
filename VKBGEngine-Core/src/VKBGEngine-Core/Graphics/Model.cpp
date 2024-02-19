#include "Model.h"
#include "RenderContext.h"

namespace vkbg
{
    Model::Model(RenderContext* context, const std::vector<Vertex>& vertices)
        : m_Context(context)
    {
        CreateVertexBuffers(vertices);
    }

    Model::~Model()
    {
        vkDestroyBuffer(m_Context->GetLogicalDevice(), m_VertexBuffer, nullptr);
        vkFreeMemory(m_Context->GetLogicalDevice(), m_VertexBufferMemory, nullptr);
    }

    void Model::Bind(VkCommandBuffer commandBuffer)
    {
        VkBuffer buffers[]{ m_VertexBuffer };
        VkDeviceSize offsets[]{ 0 };

        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
    }

    void Model::Draw(VkCommandBuffer commandBuffer)
    {
        vkCmdDraw(commandBuffer, m_VertexCount, 1, 0, 0);
    }

    void Model::CreateVertexBuffers(const std::vector<Vertex>& vertices)
    {
        m_VertexCount = (uint32_t)vertices.size();
        assert(m_VertexCount >= 3 && "vertext count must be at least 3");
        VkDeviceSize bufferSize = sizeof(vertices[0]) * m_VertexCount;

        m_Context->CreateBuffer(
            bufferSize,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, // host = CPU, Device = GPU
            m_VertexBuffer,
            m_VertexBufferMemory
        );

        void* data;
        vkMapMemory(m_Context->GetLogicalDevice(), m_VertexBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), (size_t)bufferSize);
        vkUnmapMemory(m_Context->GetLogicalDevice(), m_VertexBufferMemory);
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
                .format = VK_FORMAT_R32G32_SFLOAT,
                .offset = 0
            }
        };
        return attributeDescription;
    }
}
