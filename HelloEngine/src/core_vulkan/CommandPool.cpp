
#include "core_vulkan/CommandPool.h"
#include "core_vulkan/Manager.h"

namespace Core {
    CommandPool::CommandPool(vk::CommandPoolCreateFlags flags) 
    {
        vk::CommandPoolCreateInfo command_pool_create_info {};
        command_pool_create_info.setFlags(flags)
            .setQueueFamilyIndex(g_manager->m_device->m_graphics_family_index);
        command_pool = g_manager->m_device->m_device.createCommandPool(command_pool_create_info);
    }

    CommandPool::~CommandPool() {
        g_manager->m_device->m_device.destroyCommandPool(command_pool);
    }

    std::vector<vk::CommandBuffer> CommandPool::allocate_command_buffer(uint32_t count) {
        vk::CommandBufferAllocateInfo command_buffer_allocate_info {};
        command_buffer_allocate_info.setCommandPool(command_pool)
            .setLevel(vk::CommandBufferLevel::ePrimary)
            .setCommandBufferCount(count);
        return std::move(g_manager->m_device->m_device.allocateCommandBuffers(command_buffer_allocate_info));
    }

    vk::CommandBuffer CommandPool::allocate_single_use() {
        vk::CommandBufferAllocateInfo command_buffer_allocate_info {};
        command_buffer_allocate_info.setCommandPool(command_pool)
            .setLevel(vk::CommandBufferLevel::ePrimary)
            .setCommandBufferCount(1);
        auto command_buffer = g_manager->m_device->m_device.allocateCommandBuffers(command_buffer_allocate_info)[0];

        vk::CommandBufferBeginInfo begin_info {};
        begin_info.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        command_buffer.begin(begin_info);

        return command_buffer;
    }

    void CommandPool::free_single_use(vk::CommandBuffer command_buffer) {
        command_buffer.end();
        vk::SubmitInfo submit_info {};
        submit_info.setCommandBuffers(command_buffer);
            // .setCommandBufferCount(1)
            // .setPCommandBuffers(&command_buffer)
        g_manager->m_device->m_transfer_queue.submit({ submit_info }, VK_NULL_HANDLE);
        g_manager->m_device->m_transfer_queue.waitIdle();
        g_manager->m_device->m_device.freeCommandBuffers(command_pool, { command_buffer });
    }
}
