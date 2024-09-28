
#include "core_vulkan/Manager.h"
#include "core_impl/EngineLoggerImpl.h"
#include "core_impl/Setting.h"
//#include "vulkan/vulkan.hpp"
#include "GLFW/glfw3.h"
#include "core_impl/window.h"


namespace Core
{
	APIManager* g_manager;

    std::unique_ptr<APIManager> APIManager::s_manager;

    APIManager &APIManager::GetInstance() {
        if (s_manager.get() == nullptr) {
            s_manager.reset(new APIManager);
        }
        return *s_manager;
    }

    void APIManager::Quit() {
        s_manager.reset();
    }

    APIManager::APIManager() 
    {
        CreateVKInstance();
        if (g_setting.m_enable_ray_tracing) {
            vk::DynamicLoader dl;
            auto vkGetInstanceProcAddr = dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
            m_dispatcher = vk::DispatchLoaderDynamic(m_instance, vkGetInstanceProcAddr);
        }

        this->m_runtime_setting = std::make_shared<RuntimeSetting>();
        ::Core::g_manager = this;
    }

    APIManager::~APIManager() {
        Core::g_manager = nullptr;
    }

    std::vector<std::string> APIManager::EnumerateDecices() 
    {
        return m_device->EnumerateDevices();
    }

    std::shared_ptr<RuntimeSetting> APIManager::GetRuntimeSetting() 
    {
        return m_runtime_setting;
    }

    std::shared_ptr<ResourceFactory> APIManager::CreateResourceFactory(const std::string &root) 
    {
        return std::make_shared<ResourceFactory>(root);
    }

    CommandPool &APIManager::GetCommandPool() 
    {
        return *m_command_pool;
    }

    void APIManager::Initialize() {
        LogInfo("Initialize Vulkan API")
        CreateVKSurface();
        m_device = std::make_unique<Device>();
        InitializeVMA();
        m_swapchain = std::make_unique<Swapchain>();
        m_command_pool = std::make_unique<CommandPool>(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
        m_descriptor_pool = std::make_unique<DescriptorPool>();
    }

    void APIManager::CreateVKInstance() 
    {
        vk::ApplicationInfo app_info {};
        app_info.setPApplicationName(g_setting.m_app_name.c_str())
            .setApplicationVersion(g_setting.m_app_version)
            .setPEngineName(g_setting.m_engine_name.c_str())
            .setEngineVersion(g_setting.m_engine_version)
            .setApiVersion(vk::ApiVersion13);
        vk::InstanceCreateInfo instance_create_info {};
        instance_create_info.setPApplicationInfo(&app_info);

        std::map<std::string, bool> required_extensions;
        uint32_t count;
        const char **glfw_required_extensions = glfwGetRequiredInstanceExtensions(&count);
        for (uint32_t i = 0; i < count; i ++) {
            required_extensions.insert(std::make_pair(std::string(glfw_required_extensions[i]), false));
        }

        std::vector<const char *> enabled_extensions;
        auto extensions = vk::enumerateInstanceExtensionProperties();
        bool found = false;
        for (auto &extension : extensions) {
            for (auto &required_extension : required_extensions) {
                if (required_extension.first == extension.extensionName) {
                    required_extension.second = true;
                    enabled_extensions.push_back(required_extension.first.c_str());
                    break;
                }
            }
            LogTrace("EXT: {}-{}", std::string(extension.extensionName), extension.specVersion)
        }

        for (auto &required_extension : required_extensions) {
            if (!required_extension.second) {
                LogError("Faild find required vulkan extension: {}", required_extension.first)
            }
        }

        instance_create_info.setPEnabledExtensionNames(enabled_extensions);

        const char *validation_layers[] = { "VK_LAYER_KHRONOS_validation" };
        if (g_setting.m_debug_mode) 
        {
            auto layers = vk::enumerateInstanceLayerProperties();
            bool found = false;
            for (auto &layer : layers) 
            {
                if (strcmp(validation_layers[0], layer.layerName) == 0) 
                {
                    found = true;
                    instance_create_info.setPEnabledLayerNames(validation_layers);
                }
                LogTrace("LYR: {} \"{}\" {}-{}", std::string(layer.layerName), std::string(layer.description), layer.implementationVersion, layer.specVersion)
            }
            if (found == false) 
            {
                LogError("Cannot find Vulkan Validation Layer")
            }
        }

        m_instance = vk::createInstance(instance_create_info);
    }

    void APIManager::CreateVKSurface() {
        VkSurfaceKHR surface;
        glfwCreateWindowSurface(m_instance, g_window->get_glfw_window(), nullptr, &surface);
        this->m_surface = surface;
    }

    void APIManager::InitializeVMA() {
        VmaAllocatorCreateInfo create_info {};
        if (g_setting.m_enable_ray_tracing) {
            create_info.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
        }
        create_info.vulkanApiVersion = VK_API_VERSION_1_3;
        create_info.instance = m_instance;
        create_info.physicalDevice = m_device->m_physical_device;
        create_info.device = m_device->m_device;
        vmaCreateAllocator(&create_info, &m_vma_allocator);
    }

    void APIManager::RecreateSwapchin() {
        vkDeviceWaitIdle(m_device->m_device);
        m_swapchain.reset();
        int width, height;
        glfwGetWindowSize(g_window->get_glfw_window(), &width, &height);
        while (width == 0 || height == 0) 
        {
            glfwGetWindowSize(g_window->get_glfw_window(), &width, &height);
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
        m_swapchain = std::make_unique<Swapchain>();
    }

    void APIManager::Destroy() {
        LogInfo("Destroy Vulkan API")
        m_descriptor_pool.reset();
        m_command_pool.reset();
        m_swapchain.reset();
        vmaDestroyAllocator(m_vma_allocator);
        m_device.reset();
        this->m_runtime_setting.reset();
        m_instance.destroySurfaceKHR(m_surface);
        m_instance.destroy();
    }

} // namespace Croe
