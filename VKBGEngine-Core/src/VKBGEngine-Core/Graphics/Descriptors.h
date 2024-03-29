#pragma once

namespace vkbg
{
class DescriptorSetLayout
{
public:
    class Builder
    {
    public:
        Builder(class RenderContext* context) : m_Context{ context } {}

        Builder& AddBinding(
            uint32_t binding,
            VkDescriptorType descriptorType,
            VkShaderStageFlags stageFlags,
            uint32_t count = 1);
        std::unique_ptr<DescriptorSetLayout> Build() const;

    private:
        class RenderContext* m_Context;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_Bindings{};
    };

    DescriptorSetLayout(
        class RenderContext* context, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
    ~DescriptorSetLayout();
    DescriptorSetLayout(const DescriptorSetLayout&) = delete;
    DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;

    VkDescriptorSetLayout GetDescriptorSetLayout() const { return m_DescriptorSetLayout; }

private:
    class RenderContext* m_Context;
    VkDescriptorSetLayout m_DescriptorSetLayout;
    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_Bindings;

    friend class DescriptorWriter;
};

class DescriptorPool
{
public:
    class Builder
    {
    public:
        Builder(class RenderContext* context) : m_Context{ context } {}

        Builder& AddPoolSize(VkDescriptorType descriptorType, uint32_t count);
        Builder& SetPoolFlags(VkDescriptorPoolCreateFlags flags);
        Builder& SetMaxSets(uint32_t count);
        DescriptorPool* Build() const;

    private:
        class RenderContext* m_Context;
        std::vector<VkDescriptorPoolSize> m_PoolSizes{};
        uint32_t m_MaxSets = 1000;
        VkDescriptorPoolCreateFlags m_PoolFlags = 0;
    };

    DescriptorPool(
        class RenderContext* context,
        uint32_t maxSets,
        VkDescriptorPoolCreateFlags poolFlags,
        const std::vector<VkDescriptorPoolSize>& poolSizes);
    ~DescriptorPool();
    DescriptorPool(const DescriptorPool&) = delete;
    DescriptorPool& operator=(const DescriptorPool&) = delete;

    bool AllocateDescriptor(
        const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

    void FreeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

    void ResetPool();

private:
    class RenderContext* m_Context;
    VkDescriptorPool m_DescriptorPool;

    friend class DescriptorWriter;
};

class DescriptorWriter
{
public:
    DescriptorWriter(DescriptorSetLayout& setLayout, DescriptorPool& pool);

    DescriptorWriter& WriteBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
    DescriptorWriter& WriteImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

    bool Build(VkDescriptorSet& set);
    void Overwrite(VkDescriptorSet& set);

private:
    DescriptorSetLayout& m_SetLayout;
    DescriptorPool& m_Pool;
    std::vector<VkWriteDescriptorSet> m_Writes;
};
}
