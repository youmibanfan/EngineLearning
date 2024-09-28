#pragma once
#include "Common.h"
#include "vulkan/vulkan.hpp"

namespace Core
{
	class Device
	{
		no_copy_move_construction(Device)

	public:
		~Device();
		Device();
		std::vector<std::string> EnumerateDevices() const;

	private:
		bool CheckDeviceSuitable(vk::PhysicalDevice device);

	INNER_VISIBLE:
		vk::PhysicalDevice m_physical_device;
		vk::Device m_device;
		vk::Queue m_graphics_queue;
		uint32_t m_graphics_family_index = -1;
		
		vk::Queue m_present_queue;
		uint32_t m_present_family_index = -1;

		vk::Queue m_compute_queue;
		uint32_t m_compute_family_index = -1;

		vk::Queue m_transfer_queue;
		uint32_t m_transfer_family_index = -1;
	};
}