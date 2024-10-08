#pragma once

#include "Common.h"
#include "vulkan/vulkan.hpp"

namespace Core 
{
    class DescriptorPool 
    {
        no_copy_move_construction(DescriptorPool)
    public:
        DescriptorPool();
        std::vector<vk::DescriptorSet> allocate_descriptor_sets(vk::DescriptorSetLayout layout);
        void free_descriptor_sets(const std::vector<vk::DescriptorSet> &sets);
        ~DescriptorPool();
    INNER_VISIBLE:
        vk::DescriptorPool descriptor_pool;
    };
}
