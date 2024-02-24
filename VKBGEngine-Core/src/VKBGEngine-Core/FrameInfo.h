#pragma once
#include "Entities/Camera.h"

namespace vkbg
{
struct FrameInfo
{
    uint32_t FrameIndex;
    float FrameTime;
    VkCommandBuffer CommandBuffer;
    Camera& CameraRef;
    VkDescriptorSet GlobalDescriptorSet;
};
}
