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
        glm::vec3 Normal;
        glm::vec2 UV;

        static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
        static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();

        bool operator==(const Vertex& other) const
        {
            return Position == other.Position
                && Color == other.Color
                && Normal == other.Normal
                && UV == other.UV;
        }
    };

    struct Builder
    {
        std::vector<Vertex> Vertices;
        std::vector<uint32_t> Indices;

        void LoadFromObj(const std::string& filePath);
    };

public:
    Model(class RenderContext* context, const Builder& builder);
    ~Model();

    Model(const Model&) = delete;
    Model& operator=(const Model&) = delete;

    void Bind(VkCommandBuffer commandBuffer);
    void Draw(VkCommandBuffer commandBuffer);

    static std::unique_ptr<Model> CreateModelFromObj(class RenderContext* context, const std::string& filePath);

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
