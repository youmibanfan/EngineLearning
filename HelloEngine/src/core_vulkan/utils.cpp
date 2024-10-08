
#pragma once
#include "core_vulkan/utils.h"
#include "core_vulkan/Manager.h"
#include "core_vulkan/Manager.h"
#include "core_impl/EngineLoggerImpl.h"

namespace Core
{
    vk::ImageView create_image_view(vk::Image image, vk::Format format, vk::ImageAspectFlags aspect, uint32_t mipmap_levels, uint32_t layer_count, vk::ImageViewType view_type) {
        vk::ImageViewCreateInfo create_info {};
        create_info.setImage(image)
            .setFormat(format)
            .setViewType(view_type)
            .setComponents({
                vk::ComponentSwizzle::eR,
                vk::ComponentSwizzle::eG,
                vk::ComponentSwizzle::eB,
                vk::ComponentSwizzle::eA,
            })
            .setSubresourceRange({
                aspect,
                0,
                mipmap_levels,
                0,
                layer_count
            });
        return g_manager->m_device->m_device.createImageView(create_info);
    }

    vk::Format find_supported_format(const std::vector<vk::Format> &candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features) {
        for (const auto &format : candidates) {
            auto properties = g_manager->m_device->m_physical_device.getFormatProperties(format);
            if (tiling == vk::ImageTiling::eLinear && (properties.linearTilingFeatures & features) == features) {
                return format;
            } else if (tiling == vk::ImageTiling::eOptimal && (properties.optimalTilingFeatures & features) == features) {
                return format;
            }
        }
        LogError("Failed to find supported vulkan format")
        return vk::Format::eUndefined;
    }

    void transition_image_layout(vk::Image image, vk::ImageAspectFlags aspect, uint32_t mip_levels, const TransitionInfo &src, const TransitionInfo &dst) {
        auto command_buffer = g_manager->m_command_pool->allocate_single_use();
        vk::ImageMemoryBarrier barrier {};
        barrier.setOldLayout(src.layout)
            .setSrcAccessMask(src.access)
            .setNewLayout(dst.layout)
            .setDstAccessMask(dst.access)
            .setImage(image)
            .setSubresourceRange({
                aspect,
                0,
                mip_levels,
                0,
                1
            })
            .setSrcQueueFamilyIndex(vk::QueueFamilyIgnored)
            .setDstQueueFamilyIndex(vk::QueueFamilyIgnored);
        command_buffer.pipelineBarrier(
            src.stage,
            dst.stage,
            vk::DependencyFlagBits::eByRegion,
            {}, {},
            { barrier }
        );
        g_manager->m_command_pool->free_single_use(command_buffer);
    }

    vk::Format get_supported_depth_format() {
        return find_supported_format({ vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint }, vk::ImageTiling::eOptimal, vk::FormatFeatureFlagBits::eDepthStencilAttachment);
    }

    bool has_stencil_component(vk::Format format) {
        return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
    }

    SampleCount get_max_usable_sample_count() {
        auto properties = g_manager->m_device->m_physical_device.getProperties();

        vk::SampleCountFlags counts = properties.limits.framebufferColorSampleCounts & properties.limits.framebufferDepthSampleCounts;
        if (counts & vk::SampleCountFlagBits::e64) { return SampleCount::e64; }
        if (counts & vk::SampleCountFlagBits::e32) { return SampleCount::e32; }
        if (counts & vk::SampleCountFlagBits::e16) { return SampleCount::e16; }
        if (counts & vk::SampleCountFlagBits::e8) { return SampleCount::e8; }
        if (counts & vk::SampleCountFlagBits::e4) { return SampleCount::e4; }
        if (counts & vk::SampleCountFlagBits::e2) { return SampleCount::e2; }

        return SampleCount::e1;
    }

    vk::DeviceAddress get_buffer_address(vk::Buffer buffer) {
        vk::BufferDeviceAddressInfo info {};
        info.setBuffer(buffer);
        return g_manager->m_device->m_device.getBufferAddress(info);
    }

    vk::DeviceAddress get_acceleration_structure_address(vk::AccelerationStructureKHR acceleration_structure) {
        vk::AccelerationStructureDeviceAddressInfoKHR info {};
        info.setAccelerationStructure(acceleration_structure);
        return g_manager->m_device->m_device.getAccelerationStructureAddressKHR(info, g_manager->m_dispatcher);
    }
}
