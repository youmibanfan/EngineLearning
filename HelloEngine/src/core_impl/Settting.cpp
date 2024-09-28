#include "core_impl/Setting.h"
#include "core_impl/EngineLoggerImpl.h"
#include "core_vulkan/utils.h"

namespace Core
{
	Setting g_setting;
	std::shared_ptr<RuntimeSetting> g_runtime_setting;

	RuntimeSetting& RuntimeSetting::SetVSync(bool flag)
	{
		LogInfo("Set V-SYNC{}", flag);
		m_vsync = flag;
		return *this;
	}

	bool RuntimeSetting::IsMsaaEnabled()
	{
		return m_multisample_count != SampleCount::e1;
	}

	RuntimeSetting& RuntimeSetting::SetMultiSampleCount(SampleCount count)
	{
		static auto max_multisample_count = get_max_usable_sample_count();
     if (static_cast<int>(count) > static_cast<int>(max_multisample_count)) 
	 {
            LogWarn("multisample count {} > max multisample count {}", static_cast<uint32_t>(count), static_cast<uint32_t>(max_multisample_count))
            count = max_multisample_count;
        }
        m_multisample_count = count;
        return *this;
	}
}
