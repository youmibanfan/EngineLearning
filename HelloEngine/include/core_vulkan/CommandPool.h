#pragma once
#include "Common.h"
#include <vulkan/vulkan.hpp>

namespace Core {
    class CommandPool 
    {
        no_copy_move_construction(CommandPool)

    public:
        CommandPool(vk::CommandPoolCreateFlags flags);
        ~CommandPool();
        std::vector<vk::CommandBuffer> allocate_command_buffer(uint32_t count);
        vk::CommandBuffer allocate_single_use();
        void free_single_use(vk::CommandBuffer command_buffer);

    INNER_VISIBLE:
        vk::CommandPool command_pool;
    };
}
