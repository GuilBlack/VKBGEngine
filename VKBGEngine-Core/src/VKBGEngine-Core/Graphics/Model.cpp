#include "Model.h"
#include "RenderContext.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"

#include "Helper.h"

namespace std
{
template<>
struct hash <vkbg::Model::Vertex>
{
    size_t operator()(const vkbg::Model::Vertex& vertex) const
    {
        size_t seed = 0;
        vkbg::HashCombine(seed, vertex.Position, vertex.Color, vertex.Normal, vertex.UV);
        return seed;
    }
};
}

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

std::unique_ptr<Model> Model::CreateModelFromObj(RenderContext* context, const std::string& filePath)
{
    Builder builder{};
    builder.LoadFromObj(filePath);
    std::cout << "vertices count: " << builder.Vertices.size() << '\n';
    return std::make_unique<Model>(context, builder);
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
        },
        {
            .location = 2,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(Vertex, Normal)
        },
        {
            .location = 3,
            .binding = 0,
            .format = VK_FORMAT_R32G32_SFLOAT,
            .offset = offsetof(Vertex, UV)
        }
    };
    return attributeDescription;
}

////////////////////////////////////////////////////////
// Builder /////////////////////////////////////////////
////////////////////////////////////////////////////////
void Model::Builder::LoadFromObj(const std::string& filePath)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filePath.data()) == false)
        throw std::runtime_error(warn + err);

    Vertices.clear();
    Indices.clear();

    std::unordered_map<Vertex, uint32_t> uniqueVertices{};

    for (const auto& shape : shapes)
    {
        for (const auto& index : shape.mesh.indices)
        {
            Vertex vert{};
            int32_t i{ 0 };
            
            if (index.vertex_index >= 0)
            {
                i = index.vertex_index;

                vert.Position = {
                    attrib.vertices[3 * i + 0],
                    attrib.vertices[3 * i + 1],
                    attrib.vertices[3 * i + 2]
                };

                vert.Color = {
                    attrib.colors[3 * i + 0],
                    attrib.colors[3 * i + 1],
                    attrib.colors[3 * i + 2]
                };
            }
            
            if (index.normal_index >= 0)
            {
                i = index.normal_index;
                vert.Normal = {
                    attrib.normals[3 * i + 0],
                    attrib.normals[3 * i + 1],
                    attrib.normals[3 * i + 2],
                };
            }

            if (index.texcoord_index >= 0)
            {
                i = index.texcoord_index;
                vert.UV = {
                    attrib.texcoords[2 * i + 0],
                    attrib.texcoords[2 * i + 1],
                };
            }

            if (uniqueVertices.count(vert) == 0)
            {
                uniqueVertices[vert] = (uint32_t)Vertices.size();
                Vertices.push_back(vert);
            }
            Indices.push_back(uniqueVertices[vert]);
        }
    }
}
}
