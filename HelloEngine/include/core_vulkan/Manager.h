#pragma once

#include "core_vulkan/VulkanCommons.h"
#include "core_vulkan/Device.h"
#include "core_impl/Setting.h"
#include "core_vulkan/ResourceFactory.h"
#include "core_vulkan/CommandPool.h"
#include "core_vulkan/Swapchain.h"
#include "core_vulkan/descriptor_resource/DescriptorPool.h"

class Renderer;
namespace Core
{
	class APIManager
	{
		no_copy_move_construction(APIManager);
		friend std::vector<std::string> FEnumerateDevices();
		friend APIManager& FInitialize();
		friend void FDestory();
		friend Renderer; // to do 

	private:
		APIManager();
	public:
		std::vector<std::string> EnumerateDecices();
		void Initialize();
			
		std::shared_ptr<RuntimeSetting> GetRuntimeSetting();
		std::shared_ptr<ResourceFactory> CreateResourceFactory(const std::string& root);

		CommandPool& GetCommandPool();
		void Destroy();

	private:
		static APIManager& GetInstance();
		static void Quit();

	private:
		void RecreateSwapchin();

	private:
		void CreateVKInstance();
		void CreateVKSurface();
		void InitializeVMA();

	private:
		static std::unique_ptr<APIManager> s_manager;

	INNER_VISIBLE:
		vk::DispatchLoaderDynamic m_dispatcher;
		VmaAllocator m_vma_allocator;
		vk::Instance m_instance;
		vk::SurfaceKHR m_surface;

		std::shared_ptr<RuntimeSetting> m_runtime_setting;
		std::unique_ptr<Device> m_device;
		std::unique_ptr<Swapchain> m_swapchain;
		std::unique_ptr<CommandPool> m_command_pool;
		std::unique_ptr<DescriptorPool> m_descriptor_pool;

	};
	extern APIManager* g_manager;

}
