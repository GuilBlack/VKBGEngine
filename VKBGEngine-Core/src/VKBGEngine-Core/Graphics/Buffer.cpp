/*
 * Encapsulates a vulkan buffer
 *
 * Initially based off VulkanBuffer by Sascha Willems -
 * https://github.com/SaschaWillems/Vulkan/blob/master/base/VulkanBuffer.h
 */

#include "Buffer.h"
#include "RenderContext.h"

namespace vkbg
{
    /**
 * Returns the minimum instance size required to be compatible with devices minOffsetAlignment
 *
 * @param instanceSize The size of an instance
 * @param minOffsetAlignment The minimum required alignment, in bytes, for the offset member (eg
 * minUniformBufferOffsetAlignment)
 *
 * @return VkResult of the buffer mapping call
 */
VkDeviceSize Buffer::GetAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment)
{
    if (minOffsetAlignment > 0)
    {
        return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
    }
    return instanceSize;
}

Buffer::Buffer(
    RenderContext* context,
    VkDeviceSize instanceSize,
    uint32_t instanceCount,
    VkBufferUsageFlags usageFlags,
    VkMemoryPropertyFlags memoryPropertyFlags,
    VkDeviceSize minOffsetAlignment)
    : m_Context{ context },
    m_InstanceSize{ instanceSize },
    m_InstanceCount{ instanceCount },
    m_UsageFlags{ usageFlags },
    m_MemoryPropertyFlags{ memoryPropertyFlags }
{
    m_AlignmentSize = GetAlignment(instanceSize, minOffsetAlignment);
    m_BufferSize = m_AlignmentSize * instanceCount;
    context->CreateBuffer(m_BufferSize, m_UsageFlags, memoryPropertyFlags, m_Buffer, m_Memory);
}

Buffer::~Buffer()
{
    Unmap();
    vkDestroyBuffer(m_Context->GetLogicalDevice(), m_Buffer, nullptr);
    vkFreeMemory(m_Context->GetLogicalDevice(), m_Memory, nullptr);
}

Buffer::Buffer(Buffer&& other)
{
    assert(&other != this && "You are trying to move the same buffer");

    m_Context = other.m_Context;
    m_Mapped = other.m_Mapped;
    m_Buffer = other.m_Buffer;
    m_Memory = other.m_Memory;
    m_BufferSize = other.m_BufferSize;
    m_InstanceCount = other.m_InstanceCount;
    m_InstanceSize = other.m_InstanceSize;
    m_AlignmentSize = other.m_AlignmentSize;
    m_UsageFlags = other.m_UsageFlags;
    m_MemoryPropertyFlags = other.m_MemoryPropertyFlags;
    other.m_Mapped = nullptr;
    other.m_Buffer = nullptr;
    other.m_Memory = nullptr;
}

/**
    * Map a memory range of this buffer. If successful, mapped points to the specified buffer range.
    *
    * @param size (Optional) Size of the memory range to map. Pass VK_WHOLE_SIZE to map the complete
    * buffer range.
    * @param offset (Optional) Byte offset from beginning
    *
    * @return VkResult of the buffer mapping call
    */
VkResult Buffer::Map(VkDeviceSize size, VkDeviceSize offset)
{
    assert(m_Buffer && m_Memory && "Called map on buffer before create");
    return vkMapMemory(m_Context->GetLogicalDevice(), m_Memory, offset, size, 0, &m_Mapped);
}

/**
    * Unmap a mapped memory range
    *
    * @note Does not return a result as vkUnmapMemory can't fail
    */
void Buffer::Unmap()
{
    if (m_Mapped)
    {
        vkUnmapMemory(m_Context->GetLogicalDevice(), m_Memory);
        m_Mapped = nullptr;
    }
}

/**
    * Copies the specified data to the mapped buffer. Default value writes whole buffer range
    *
    * @param data Pointer to the data to copy
    * @param size (Optional) Size of the data to copy. Pass VK_WHOLE_SIZE to flush the complete buffer
    * range.
    * @param offset (Optional) Byte offset from beginning of mapped region
    *
    */
void Buffer::WriteToBuffer(void* data, VkDeviceSize size, VkDeviceSize offset)
{
    assert(m_Mapped && "Cannot copy to unmapped buffer");

    if (size == VK_WHOLE_SIZE)
    {
        memcpy(m_Mapped, data, m_BufferSize);
    }
    else
    {
        char* memOffset = (char*)m_Mapped;
        memOffset += offset;
        memcpy(memOffset, data, size);
    }
}

/**
    * Flush a memory range of the buffer to make it visible to the device
    *
    * @note Only required for non-coherent memory
    *
    * @param size (Optional) Size of the memory range to flush. Pass VK_WHOLE_SIZE to flush the
    * complete buffer range.
    * @param offset (Optional) Byte offset from beginning
    *
    * @return VkResult of the flush call
    */
VkResult Buffer::Flush(VkDeviceSize size, VkDeviceSize offset)
{
    VkMappedMemoryRange mappedRange = {};
    mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    mappedRange.memory = m_Memory;
    mappedRange.offset = offset;
    mappedRange.size = size;
    return vkFlushMappedMemoryRanges(m_Context->GetLogicalDevice(), 1, &mappedRange);
}

/**
    * Invalidate a memory range of the buffer to make it visible to the host
    *
    * @note Only required for non-coherent memory
    *
    * @param size (Optional) Size of the memory range to invalidate. Pass VK_WHOLE_SIZE to invalidate
    * the complete buffer range.
    * @param offset (Optional) Byte offset from beginning
    *
    * @return VkResult of the invalidate call
    */
VkResult Buffer::Invalidate(VkDeviceSize size, VkDeviceSize offset)
{
    VkMappedMemoryRange mappedRange = {};
    mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    mappedRange.memory = m_Memory;
    mappedRange.offset = offset;
    mappedRange.size = size;
    return vkInvalidateMappedMemoryRanges(m_Context->GetLogicalDevice(), 1, &mappedRange);
}

/**
    * Create a buffer info descriptor
    *
    * @param size (Optional) Size of the memory range of the descriptor
    * @param offset (Optional) Byte offset from beginning
    *
    * @return VkDescriptorBufferInfo of specified offset and range
    */
VkDescriptorBufferInfo Buffer::DescriptorInfo(VkDeviceSize size, VkDeviceSize offset)
{
    return VkDescriptorBufferInfo{
        m_Buffer,
        offset,
        size,
    };
}

/**
    * Copies "instanceSize" bytes of data to the mapped buffer at an offset of index * alignmentSize
    *
    * @param data Pointer to the data to copy
    * @param index Used in offset calculation
    *
    */
void Buffer::WriteToIndex(void* data, int index)
{
    WriteToBuffer(data, m_InstanceSize, index * m_AlignmentSize);
}

/**
    *  Flush the memory range at index * alignmentSize of the buffer to make it visible to the device
    *
    * @param index Used in offset calculation
    *
    */
VkResult Buffer::FlushIndex(int index) { return Flush(m_AlignmentSize, index * m_AlignmentSize); }

/**
    * Create a buffer info descriptor
    *
    * @param index Specifies the region given by index * alignmentSize
    *
    * @return VkDescriptorBufferInfo for instance at index
    */
VkDescriptorBufferInfo Buffer::DescriptorInfoForIndex(int index)
{
    return DescriptorInfo(m_AlignmentSize, index * m_AlignmentSize);
}

/**
    * Invalidate a memory range of the buffer to make it visible to the host
    *
    * @note Only required for non-coherent memory
    *
    * @param index Specifies the region to invalidate: index * alignmentSize
    *
    * @return VkResult of the invalidate call
    */
VkResult Buffer::InvalidateIndex(int index)
{
    return Invalidate(m_AlignmentSize, index * m_AlignmentSize);
}
}
