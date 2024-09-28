
#include "core_vulkan/Device.h"
#include "core_vulkan/Manager.h"
#include "core_impl/Setting.h"
#include "Constant.h"
#include <set>
#include <map>

namespace Core
{
	std::vector<std::string> Device::EnumerateDevices() const
	{
		auto devices = g_manager->m_instance.enumeratePhysicalDevices();
		std::vector<std::string> device_names;
		vk::PhysicalDeviceProperties properties;
		auto index = 0;

		for (auto& device : devices)
		{
			properties = device.getProperties();
			device_names.push_back(properties.deviceName);
			LogWarn("No. {}  \"{}\" ", index, device_names.back());
			index++;
		}
		return device_names;
	}

	Device::Device()
	{
        bool selected_device = false;
        if (g_setting.m_device_name != AUTO_SELECT_DEVICE) 
        {
            auto devices = g_manager->m_instance.enumeratePhysicalDevices();
            vk::PhysicalDeviceProperties properties;
            bool found_device = false;
            for (auto &device : devices) 
            {
                properties = device.getProperties();
                if (properties.deviceName.data() == g_setting.m_device_name) 
                {
                    LogDebug("Found device {}", std::string(properties.deviceName))
                    if (CheckDeviceSuitable(device)) 
                    {
                        selected_device = true;
                    }
                    found_device = true;
                    break;
                }
            }
            if (!found_device) 
            {
                LogDebug("Cannot find device {}", g_setting.m_device_name)
            }
        }
        if ((g_setting.m_device_name == AUTO_SELECT_DEVICE) || (!selected_device)) 
        {
            LogDebug("Auto find device")
            selected_device = false;
            auto devices = g_manager->m_instance.enumeratePhysicalDevices();
            for (auto &device : devices) 
            {
                if (CheckDeviceSuitable(device)) 
                {
                    selected_device = true;
                    break;
                }
            }
            if (!selected_device) 
            {
                LogDebug("Cannot find suitable device on this pc")
            }
        }

        std::set<uint32_t> queue_family_indices = 
        {   m_graphics_family_index, 
            m_present_family_index, 
            m_transfer_family_index, 
            m_compute_family_index
        };

        std::vector<vk::DeviceQueueCreateInfo>
            queue_create_infos(queue_family_indices.size());
        uint32_t i = 0;
        float priorities = 1.0f;
        for (auto queue_family_index : queue_family_indices) {
            queue_create_infos[i].setQueueFamilyIndex(queue_family_index)
                .setQueueCount(1)
                .setQueuePriorities(priorities);
            i ++;
        }

        vk::PhysicalDeviceFeatures features {};
        features.samplerAnisotropy = VK_TRUE;
        features.depthClamp = VK_TRUE;
        features.sampleRateShading = VK_TRUE;
        features.shaderInt64 = VK_TRUE;
        features.fragmentStoresAndAtomics = VK_TRUE;
        features.multiDrawIndirect = VK_TRUE;
        features.geometryShader = VK_TRUE;
        features.fillModeNonSolid = VK_TRUE;
        features.wideLines = VK_TRUE;
        vk::PhysicalDeviceVulkan12Features vk12_features {};
        vk12_features.runtimeDescriptorArray = VK_TRUE;
        vk12_features.bufferDeviceAddress = VK_TRUE;
        vk12_features.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
        vk12_features.descriptorBindingVariableDescriptorCount = VK_TRUE;
        vk12_features.drawIndirectCount = VK_TRUE;
        vk12_features.samplerFilterMinmax = VK_TRUE;
        vk::DeviceCreateInfo device_create_info {};
        device_create_info.setPNext(&vk12_features);

        std::map<std::string, bool> required_extensions = 
        {
            { VK_KHR_SWAPCHAIN_EXTENSION_NAME, false },
            { VK_KHR_BIND_MEMORY_2_EXTENSION_NAME, false }
        };

        vk::PhysicalDeviceAccelerationStructureFeaturesKHR acceleration_structure_features {};
        vk::PhysicalDeviceRayTracingPipelineFeaturesKHR ray_tracing_pipeline_features {};
        vk::PhysicalDeviceRayQueryFeaturesKHR ray_query_features {};
        if (g_setting.m_enable_ray_tracing) {
            required_extensions.insert(std::make_pair(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME, false));
            required_extensions.insert(std::make_pair(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME, false));
            required_extensions.insert(std::make_pair(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME, false));
            required_extensions.insert(std::make_pair(VK_KHR_RAY_QUERY_EXTENSION_NAME, false));

            acceleration_structure_features.setAccelerationStructure(VK_TRUE)
                .setAccelerationStructureCaptureReplay(VK_TRUE)
                .setAccelerationStructureIndirectBuild(VK_FALSE)
                .setAccelerationStructureHostCommands(VK_FALSE)
                .setDescriptorBindingAccelerationStructureUpdateAfterBind(VK_TRUE);
            ray_tracing_pipeline_features.setRayTracingPipeline(VK_TRUE)
                .setRayTracingPipelineShaderGroupHandleCaptureReplay(VK_FALSE)
                .setRayTracingPipelineShaderGroupHandleCaptureReplayMixed(VK_FALSE)
                .setRayTracingPipelineTraceRaysIndirect(VK_TRUE)
                .setRayTraversalPrimitiveCulling(VK_TRUE);
            ray_query_features.setRayQuery(VK_TRUE);

            device_create_info.setPNext(
                &acceleration_structure_features.setPNext(
                    &ray_tracing_pipeline_features.setPNext(
                        &ray_query_features.setPNext(&vk12_features)
                    )
                )
            );
        }

        for (const auto &extension : g_setting.m_device_extensions) {
            required_extensions.insert(std::make_pair(extension, false));
        }

        std::vector<const char *> enabled_extensions;
        auto extensions = m_physical_device.enumerateDeviceExtensionProperties();
        bool found = false;
        for (auto &extension : extensions) 
        {
            for (auto &required_extension : required_extensions) 
            {
                if (required_extension.first == extension.extensionName) 
                {
                    required_extension.second = true;
                    enabled_extensions.push_back(required_extension.first.c_str());
                    break;
                }
            }
            LogTrace("PHY EXT: {}-{}", std::string(extension.extensionName), extension.specVersion)
        }

        for (auto &required_extension : required_extensions) 
        {
            if (!required_extension.second) 
            {
                LogError("Faild find required vulkan physical device extension: {}", required_extension.first)
            }
        }

        const char *layers[] = { "VK_LAYER_KHRONOS_validation" };
        device_create_info.setQueueCreateInfos(queue_create_infos)
            .setPEnabledExtensionNames(enabled_extensions)
            .setPEnabledFeatures(&features);
        if (g_setting.m_debug_mode)
        {
            device_create_info.setPEnabledLayerNames(layers);
        }
        m_device = m_physical_device.createDevice(device_create_info);

        m_graphics_queue =  m_device.getQueue(m_graphics_family_index, 0);
        m_present_queue =   m_device.getQueue(m_present_family_index, 0);
        m_transfer_queue =  m_device.getQueue(m_transfer_family_index, 0);
        m_compute_queue =   m_device.getQueue(m_compute_family_index, 0);

        if (g_setting.m_enable_ray_tracing)
        {
            vk::PhysicalDeviceProperties2 properties {};
            vk::PhysicalDeviceAccelerationStructurePropertiesKHR acceleration_structure_properties {};
            vk::PhysicalDeviceRayTracingPipelinePropertiesKHR ray_tracing_pipeline_properties {};
            properties.pNext = &acceleration_structure_properties;
            acceleration_structure_properties.pNext = &ray_tracing_pipeline_properties;
            m_physical_device.getProperties2(&properties);
            LogTrace("Max Ray Recursion Depth: {}", ray_tracing_pipeline_properties.maxRayRecursionDepth);
        }
	}

	Device::~Device() 
    {
        m_device.destroy();
    }

    bool Device::CheckDeviceSuitable(vk::PhysicalDevice device) 
    {
        auto properties = device.getProperties();
        auto features = device.getFeatures();

        if (properties.deviceType != vk::PhysicalDeviceType::eDiscreteGpu) 
        {
            LogDebug("{} is not a discrete gpu -- skipping", std::string(properties.deviceName))
            return false;
        }

        if (!features.samplerAnisotropy) 
        {
            LogDebug("{} does not support sampler anisotropy -- skipping", std::string(properties.deviceName))
            return false;
        }

        if (!features.sampleRateShading) 
        {
            LogDebug("{} does not support sampler rate shading -- skipping", std::string(properties.deviceName))
            return false;
        }

        if (!features.shaderInt64) 
        {
            LogDebug("{} does not support type int 64 -- warning", std::string(properties.deviceName))
        }

        auto queue_families_properties = device.getQueueFamilyProperties();
        auto queue_family_idx = 0;
        bool found_family = false;
        for (const auto &queue_family_properties : queue_families_properties) 
        {
            if ((queue_family_properties.queueFlags & (vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eTransfer | vk::QueueFlagBits::eCompute)) 
                &&
                (device.getSurfaceSupportKHR(queue_family_idx, g_manager->m_surface))) 
            {
                m_graphics_family_index = queue_family_idx;
                m_present_family_index = queue_family_idx;
                m_compute_family_index = queue_family_idx;
                m_transfer_family_index = queue_family_idx;
                found_family = true;
                break;
            }
            queue_family_idx++;
        }

        if (!found_family) {
            LogDebug("{} has no suitable queue family -- skipping", std::string(properties.deviceName))
            return false;
        }

        LogDebug("{} is suitable", std::string(properties.deviceName))
        m_physical_device = device;
        LogInfo("Select Device {}", std::string(properties.deviceName))

        auto memoryProperties = m_physical_device.getMemoryProperties();
        LogDebug("\tDevice Driver Version: {}.{}.{}", VK_VERSION_MAJOR(properties.driverVersion), VK_VERSION_MINOR(properties.driverVersion), VK_VERSION_PATCH(properties.driverVersion))
        LogDebug("\tVulkan API Version: {}.{}.{}", VK_VERSION_MAJOR(properties.apiVersion), VK_VERSION_MINOR(properties.apiVersion), VK_VERSION_PATCH(properties.apiVersion))
        for (uint32_t i = 0; i < memoryProperties.memoryHeapCount; i++) 
        {
            float size = ((float)memoryProperties.memoryHeaps[i].size) / 1024.0f / 1024.0f / 1024.0f;
            if (memoryProperties.memoryHeaps[i].flags & vk::MemoryHeapFlagBits::eDeviceLocal) 
            {
                LogDebug("\tLocal GPU Memory: {} Gib", size)
            }
            else {
                LogDebug("\tShared GPU Memory: {} Gib", size)
            }
        }
        return true;
    }

}