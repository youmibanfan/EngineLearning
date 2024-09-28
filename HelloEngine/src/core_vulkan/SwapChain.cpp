
#include "core_vulkan/Swapchain.h"

#include "core_impl/Setting.h"
#include "core_impl/EngineLoggerImpl.h"
#include "core_vulkan/Manager.h"
#include "core_vulkan/utils.h"


namespace Core {
    Swapchain::Swapchain() {
        SwapchainDetails details;
        query_swapchain_details(details);

        uint32_t width = std::clamp<uint32_t>(details.capabilities.currentExtent.width, details.capabilities.minImageExtent.width, details.capabilities.maxImageExtent.width);
        uint32_t height = std::clamp<uint32_t>(details.capabilities.currentExtent.height, details.capabilities.minImageExtent.height, details.capabilities.maxImageExtent.height);
        g_runtime_setting->resize({ width, height });

        format = details.formats[0];
        present_mode = details.present_modes[0];

        vk::SurfaceFormatKHR expect_format;
        if (g_setting.m_expect_format.has_value()) {
            expect_format = g_setting.m_expect_format.value();
        } else {
            expect_format.format = vk::Format::eB8G8R8A8Unorm;
            expect_format.colorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;
        }
        for (const auto &format : details.formats) {
            if (format.format == expect_format.format && format.colorSpace == expect_format.colorSpace) {
                LogDebug("Found expected surface format");
                this->format = format;
                break;
            }
        }

        vk::PresentModeKHR expect_present_mode;
        if (g_setting.m_expect_present_mode.has_value()) {
            expect_present_mode = g_setting.m_expect_present_mode.value();
        } else {
            if (g_runtime_setting->IsVSync()) {
                expect_present_mode = vk::PresentModeKHR::eFifo;
            } else {
                expect_present_mode = vk::PresentModeKHR::eMailbox;
            }
        }

        bool found = false;
        for (auto present_mode : details.present_modes) {
            if (present_mode == expect_present_mode) {
                LogDebug("Found expected present mode")
                this->present_mode = present_mode;
                found = true;
                break;
            }
        }
        if (!found) {
            for (auto present_mode : details.present_modes) {
                if (present_mode == vk::PresentModeKHR::eImmediate) {
                    LogDebug("Select immediate present mode")
                    this->present_mode = vk::PresentModeKHR::eImmediate;
                    break;
                }
            }
        }

        switch (present_mode) {
        case vk::PresentModeKHR::eMailbox:
            image_count = 4;
            break;
        case vk::PresentModeKHR::eFifo:
        case vk::PresentModeKHR::eImmediate:
        default:
            image_count = 3;
        }
        image_count = std::clamp(image_count, details.capabilities.minImageCount, details.capabilities.maxImageCount);
        LogDebug("Auto set swapchain image_count to {}", image_count);

        vk::SwapchainCreateInfoKHR swapchain_create_info {};
        swapchain_create_info.setOldSwapchain(VK_NULL_HANDLE)
            .setClipped(VK_TRUE)
            .setSurface(g_manager->m_surface)
            .setPreTransform(details.capabilities.currentTransform)
            .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
            .setPresentMode(present_mode)
            .setImageFormat(format.format)
            .setImageColorSpace(format.colorSpace)
            .setImageArrayLayers(1)
            .setImageExtent({ width, height })
            .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
            .setMinImageCount(image_count);
        uint32_t queue_family_indices[] = 
        {   
            g_manager->m_device->m_graphics_family_index,
            g_manager->m_device->m_present_family_index
        };
        if (queue_family_indices[0] == queue_family_indices[1]) {
            swapchain_create_info.setImageSharingMode(vk::SharingMode::eExclusive);
        } else {
            swapchain_create_info.setImageSharingMode(vk::SharingMode::eConcurrent)
                .setQueueFamilyIndices(queue_family_indices);
        }
        swapchain = g_manager->m_device->m_device.createSwapchainKHR(swapchain_create_info);
        images = g_manager->m_device->m_device.getSwapchainImagesKHR(swapchain);
        image_views.resize(image_count);
        for (uint32_t i = 0; i < image_count; i ++) {
            image_views[i] = create_image_view(images[i], format.format, vk::ImageAspectFlagBits::eColor, 1);
        }

        switch (present_mode) {
        case vk::PresentModeKHR::eFifo:
            LogDebug("V-Sync ON (FIFO) real image_count is {}", image_count)
            break;
        case vk::PresentModeKHR::eImmediate:
            LogDebug("V-Sync OFF (IMMEDIATE) real image_count is {}", image_count)
            break;
        case vk::PresentModeKHR::eMailbox:
            LogDebug("V-Sync OFF (MailBox) real image_count is {}", image_count)
            break;
        default:
            LogDebug("Unknown present mode real image_count is {}", image_count)
        }
    }

    Swapchain::~Swapchain() {
        for (auto image_view : image_views) {
            g_manager->m_device->m_device.destroyImageView(image_view);
        }
        images.clear();
        image_views.clear();
        g_manager->m_device->m_device.destroySwapchainKHR(swapchain);
    }

    void Swapchain::query_swapchain_details(SwapchainDetails &details) 
    {
        details.capabilities =  g_manager->m_device->m_physical_device.getSurfaceCapabilitiesKHR(   g_manager->m_surface);
        details.formats =       g_manager->m_device->m_physical_device.getSurfaceFormatsKHR(        g_manager->m_surface);
        details.present_modes = g_manager->m_device->m_physical_device.getSurfacePresentModesKHR(   g_manager->m_surface);
    }
}
