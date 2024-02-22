#pragma once

namespace vkbg
{
class Model
{
public:
    struct Vertex
    {
        glm::vec3 Position;
        glm::vec3 Color;

        static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
        static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
    };

    struct Builder
    {
        std::vector<Vertex> Vertices;
        std::vector<uint32_t> Indices;
    };

public:
    Model(class RenderContext* context, const Builder& builder);
    ~Model();

    Model(const Model&) = delete;
    Model& operator=(const Model&) = delete;

    void Bind(VkCommandBuffer commandBuffer);
    void Draw(VkCommandBuffer commandBuffer);


private:
    void CreateVertexBuffer(const std::vector<Vertex>& vertices);
    void CreateIndexBuffer(const std::vector<uint32_t>& indices);
    class RenderContext* m_Context;

    VkBuffer m_VertexBuffer{ nullptr };
    VkDeviceMemory m_VertexBufferMemory{ nullptr };
    uint32_t m_VertexCount;

    bool m_HasIndexBuffer{ false };
    VkBuffer m_IndexBuffer{ nullptr };
    VkDeviceMemory m_IndexBufferMemory{ nullptr };
    uint32_t m_IndexCount{ 0 };
};
}
