
#include "core_vulkan/descriptor_resource/DescriptorPool.h"
#include "core_vulkan/Manager.h"
#include "core_impl/Setting.h"

namespace Core {
    DescriptorPool::DescriptorPool() {
        std::vector<vk::DescriptorPoolSize> pool_sizes = {};
        vk::DescriptorPoolCreateInfo pool_create_info {};
        pool_create_info.setPoolSizes(pool_sizes)
            .setMaxSets(16)
            .setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet);
        descriptor_pool = g_manager->m_device->m_device.createDescriptorPool(pool_create_info);
    }

    std::vector<vk::DescriptorSet> DescriptorPool::allocate_descriptor_sets(vk::DescriptorSetLayout layout) {
        std::vector<vk::DescriptorSet> descriptor_sets(g_setting.m_max_in_flight_frame);
        std::vector<vk::DescriptorSetLayout> layouts(g_setting.m_max_in_flight_frame, layout);
        vk::DescriptorSetAllocateInfo alloc_info {};
        alloc_info.setDescriptorPool(descriptor_pool)
            .setDescriptorSetCount(static_cast<uint32_t>(g_setting.m_max_in_flight_frame))
            .setSetLayouts(layouts);
        descriptor_sets = g_manager->m_device->m_device.allocateDescriptorSets(alloc_info);
        return descriptor_sets;
    }

    void DescriptorPool::free_descriptor_sets(const std::vector<vk::DescriptorSet> &sets) 
    {
        g_manager->m_device->m_device.freeDescriptorSets(descriptor_pool, sets);
    }

    DescriptorPool::~DescriptorPool(){
        g_manager->m_device->m_device.destroyDescriptorPool(descriptor_pool);
    }
}