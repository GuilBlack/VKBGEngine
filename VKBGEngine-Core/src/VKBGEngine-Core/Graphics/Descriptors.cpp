#include "Descriptors.h"
#include "RenderContext.h"

namespace vkbg
{
// *************** Descriptor Set Layout Builder *********************

DescriptorSetLayout::Builder& DescriptorSetLayout::Builder::AddBinding(
    uint32_t binding,
    VkDescriptorType descriptorType,
    VkShaderStageFlags stageFlags,
    uint32_t count)
{
    assert(m_Bindings.count(binding) == 0 && "Binding already in use");
    VkDescriptorSetLayoutBinding layoutBinding{};
    layoutBinding.binding = binding;
    layoutBinding.descriptorType = descriptorType;
    layoutBinding.descriptorCount = count;
    layoutBinding.stageFlags = stageFlags;
    m_Bindings[binding] = layoutBinding;
    return *this;
}

std::unique_ptr<DescriptorSetLayout> DescriptorSetLayout::Builder::Build() const
{
    return std::make_unique<DescriptorSetLayout>(m_Context, m_Bindings);
}

// *************** Descriptor Set Layout *********************

DescriptorSetLayout::DescriptorSetLayout(
    RenderContext* context, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings)
    : m_Context{ context }, m_Bindings{ bindings }
{
    std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
    for (auto kv : bindings)
    {
        setLayoutBindings.push_back(kv.second);
    }

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
    descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
    descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

    if (vkCreateDescriptorSetLayout(
        m_Context->GetLogicalDevice(),
        &descriptorSetLayoutInfo,
        nullptr,
        &m_DescriptorSetLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

DescriptorSetLayout::~DescriptorSetLayout()
{
    vkDestroyDescriptorSetLayout(m_Context->GetLogicalDevice(), m_DescriptorSetLayout, nullptr);
}

// *************** Descriptor Pool Builder *********************

DescriptorPool::Builder& DescriptorPool::Builder::AddPoolSize(
    VkDescriptorType descriptorType, uint32_t count)
{
    m_PoolSizes.push_back({ descriptorType, count });
    return *this;
}

DescriptorPool::Builder& DescriptorPool::Builder::SetPoolFlags(
    VkDescriptorPoolCreateFlags flags)
{
    m_PoolFlags = flags;
    return *this;
}
DescriptorPool::Builder& DescriptorPool::Builder::SetMaxSets(uint32_t count)
{
    m_MaxSets = count;
    return *this;
}

DescriptorPool* DescriptorPool::Builder::Build() const
{
    return new DescriptorPool(m_Context, m_MaxSets, m_PoolFlags, m_PoolSizes);
}

// *************** Descriptor Pool *********************

DescriptorPool::DescriptorPool(
    RenderContext* context,
    uint32_t maxSets,
    VkDescriptorPoolCreateFlags poolFlags,
    const std::vector<VkDescriptorPoolSize>& poolSizes)
    : m_Context{ context }
{
    VkDescriptorPoolCreateInfo descriptorPoolInfo{};
    descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    descriptorPoolInfo.pPoolSizes = poolSizes.data();
    descriptorPoolInfo.maxSets = maxSets;
    descriptorPoolInfo.flags = poolFlags;

    if (vkCreateDescriptorPool(m_Context->GetLogicalDevice(), &descriptorPoolInfo, nullptr, &m_DescriptorPool) !=
        VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

DescriptorPool::~DescriptorPool()
{
    vkDestroyDescriptorPool(m_Context->GetLogicalDevice(), m_DescriptorPool, nullptr);
}

bool DescriptorPool::AllocateDescriptor(
    const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const
{
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_DescriptorPool;
    allocInfo.pSetLayouts = &descriptorSetLayout;
    allocInfo.descriptorSetCount = 1;

    // Might want to create a "DescriptorPoolManager" class that handles this case, and builds
    // a new pool whenever an old pool fills up. But this is beyond our current scope
    if (vkAllocateDescriptorSets(m_Context->GetLogicalDevice(), &allocInfo, &descriptor) != VK_SUCCESS)
    {
        return false;
    }
    return true;
}

void DescriptorPool::FreeDescriptors(std::vector<VkDescriptorSet>& descriptors) const
{
    vkFreeDescriptorSets(
        m_Context->GetLogicalDevice(),
        m_DescriptorPool,
        static_cast<uint32_t>(descriptors.size()),
        descriptors.data());
}

void DescriptorPool::ResetPool()
{
    vkResetDescriptorPool(m_Context->GetLogicalDevice(), m_DescriptorPool, 0);
}

// *************** Descriptor Writer *********************

DescriptorWriter::DescriptorWriter(DescriptorSetLayout& setLayout, DescriptorPool& pool)
    : m_SetLayout{ setLayout }, m_Pool{ pool }
{}

DescriptorWriter& DescriptorWriter::WriteBuffer(
    uint32_t binding, VkDescriptorBufferInfo* bufferInfo)
{
    assert(m_SetLayout.m_Bindings.count(binding) == 1 && "Layout does not contain specified binding");

    auto& bindingDescription = m_SetLayout.m_Bindings[binding];

    assert(
        bindingDescription.descriptorCount == 1 &&
        "Binding single descriptor info, but binding expects multiple");

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.descriptorType = bindingDescription.descriptorType;
    write.dstBinding = binding;
    write.pBufferInfo = bufferInfo;
    write.descriptorCount = 1;

    m_Writes.push_back(write);
    return *this;
}

DescriptorWriter& DescriptorWriter::WriteImage(
    uint32_t binding, VkDescriptorImageInfo* imageInfo)
{
    assert(m_SetLayout.m_Bindings.count(binding) == 1 && "Layout does not contain specified binding");

    auto& bindingDescription = m_SetLayout.m_Bindings[binding];

    assert(
        bindingDescription.descriptorCount == 1 &&
        "Binding single descriptor info, but binding expects multiple");

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.descriptorType = bindingDescription.descriptorType;
    write.dstBinding = binding;
    write.pImageInfo = imageInfo;
    write.descriptorCount = 1;

    m_Writes.push_back(write);
    return *this;
}

bool DescriptorWriter::Build(VkDescriptorSet& set)
{
    bool success = m_Pool.AllocateDescriptor(m_SetLayout.GetDescriptorSetLayout(), set);
    if (!success)
    {
        return false;
    }
    Overwrite(set);
    return true;
}

void DescriptorWriter::Overwrite(VkDescriptorSet& set)
{
    for (auto& write : m_Writes)
    {
        write.dstSet = set;
    }
    vkUpdateDescriptorSets(m_Pool.m_Context->GetLogicalDevice(), (uint32_t)m_Writes.size(), m_Writes.data(), 0, nullptr);
}
}
