#pragma once

//#include <Match/vulkan/commons.hpp>
#include <vulkan/vulkan.hpp>
#include "Common.h"

namespace Core {
    struct SwapchainDetails 
    {
        vk::SurfaceCapabilitiesKHR capabilities;
        std::vector<vk::SurfaceFormatKHR> formats;
        std::vector<vk::PresentModeKHR> present_modes;
    };

    class Swapchain 
    {
        no_copy_move_construction(Swapchain)
    public:
        Swapchain();
        ~Swapchain();
    private:
        void query_swapchain_details(SwapchainDetails &details);
    INNER_VISIBLE:
        vk::SwapchainKHR swapchain;
        vk::SurfaceFormatKHR format;
        vk::PresentModeKHR present_mode;
        uint32_t image_count;
        std::vector<vk::Image> images;
        std::vector<vk::ImageView> image_views;
    };
}