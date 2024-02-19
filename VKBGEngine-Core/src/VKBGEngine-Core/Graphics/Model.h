#pragma once

namespace vkbg
{
class Model
{
public:
    struct Vertex
    {
        glm::vec2 Position;

        static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
        static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
    };

public:
    Model(class RenderContext* context, const std::vector<Vertex>& vertices);
    ~Model();

    Model(const Model&) = delete;
    Model& operator=(const Model&) = delete;

    void Bind(VkCommandBuffer commandBuffer);
    void Draw(VkCommandBuffer commandBuffer);


private:
    void CreateVertexBuffers(const std::vector<Vertex>& vertices);
    class RenderContext* m_Context;
    VkBuffer m_VertexBuffer{ nullptr };
    VkDeviceMemory m_VertexBufferMemory{ nullptr };
    uint32_t m_VertexCount;
};
}
