#pragma once
//#include "core_vulkan/VulkanCommons.h"
#include "vulkan/vulkan.h"
#include "core_vulkan/VulkanType.h"

namespace Core
{
    vk::ImageView create_image_view(vk::Image image, vk::Format format, vk::ImageAspectFlags aspect, uint32_t mipmap_levels, uint32_t layer_count = 1, vk::ImageViewType view_type = vk::ImageViewType::e2D);

    vk::Format find_supported_format(const std::vector<vk::Format> &candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features);

    struct TransitionInfo {
        vk::ImageLayout layout;
        vk::AccessFlags access;
        vk::PipelineStageFlags stage;
    };

    void transition_image_layout(vk::Image image, vk::ImageAspectFlags aspect, uint32_t mip_levels, const TransitionInfo &src, const TransitionInfo &dst);

    vk::Format get_supported_depth_format();

    bool has_stencil_component(vk::Format format);

    SampleCount get_max_usable_sample_count();

    vk::DeviceAddress get_buffer_address(vk::Buffer buffer);

    vk::DeviceAddress get_acceleration_structure_address(vk::AccelerationStructureKHR acceleration_structure);
}
