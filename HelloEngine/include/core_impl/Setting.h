#pragma once


//#include <core_vulkan/VulkanCommons.h>

#include  <vulkan/vulkan.hpp>
#include <string>
#include <optional>

#include "Common.h"
#include "type_data.h"
#include "core_vulkan/VulkanType.h"
namespace Core
{
	struct Setting
	{
		bool m_debug_mode = false;
		std::string m_device_name = "core auto";
		std::string m_app_name = " Hello App";
		uint32_t m_app_version = 1;
		std::string m_engine_name = " No Engine";
		uint32_t m_engine_version = 1;
		std::optional<vk::SurfaceFormatKHR> m_expect_format;
		std::optional<vk::PresentModeKHR> m_expect_present_mode;
		std::array<uint32_t, 2> m_window_pos = { 100, 50 };
		std::array<uint32_t, 2> m_window_size = { 800, 800 };
		uint32_t m_max_in_flight_frame = 2;
		std::string m_default_font_filename = "";
		std::string m_chinese_font_filename = "";
		float m_font_size = 13.0f;
		bool m_enable_ray_tracing = false;
		std::vector<std::string> m_device_extensions{};
	};

	class RuntimeSetting
	{
		default_no_copy_move_construction(RuntimeSetting);
	public:
		RuntimeSetting& SetVSync(bool a_flag);
		bool IsVSync() const { return m_vsync; };
		const WindowSize& GetWindowSize() const { return m_window_size; }
		bool IsMsaaEnabled();
		RuntimeSetting& SetMultiSampleCount(SampleCount count);
		SampleCount GetMultiSampleCount()const { return m_multisample_count; }

	INNER_VISIBLE:
		void resize(const WindowSize& size) { m_window_size = size; }

	INNER_VISIBLE:
		bool m_vsync{ true };
		WindowSize m_window_size{};
		SampleCount m_multisample_count{ SampleCount::e1 };
		uint32_t m_current_in_flight{ 0 };

	};
	extern Setting g_setting;
	extern std::shared_ptr<RuntimeSetting> g_runtime_setting;

}