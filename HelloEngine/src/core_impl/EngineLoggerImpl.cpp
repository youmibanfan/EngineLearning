
#include "core_impl/EngineLoggerImpl.h"
#include "spdlog/sinks/stdout_color_sinks.h"



namespace Core
{
	template <>
	spdlog::level::level_enum TypeTransform<spdlog::level::level_enum, LogLevel>(LogLevel src_level)
	{
		switch (src_level)
		{
		case LogLevel::eTrace:
			return spdlog::level::trace;
		case LogLevel::eDebug:
			return spdlog::level::debug;
		case LogLevel::eInfo:
			return spdlog::level::info;
		case LogLevel::eWarn:
			return spdlog::level::warn;
		case LogLevel::eError:
			return spdlog::level::err;
		case LogLevel::eFatal:
			return spdlog::level::critical;
		}

		return spdlog::level::critical;
	};

	EngineLogger* EngineLogger::GetInstance()
	{
		static EngineLogger s_instance;
		s_instance.Initialize();
		return &s_instance;
	}

	void EngineLogger::SetLogLevel(LogLevel level)
	{
		GetInstance()->SetLevel(level);
	}

	void EngineLogger::Initialize()
	{
		if (m_is_initialized)
			return;

		m_is_initialized = true;
		m_spd_logger = spdlog::stdout_color_mt("Engine Core");
		m_spd_logger->set_level(
			TypeTransform<spdlog::level::level_enum>(m_log_level));
	}

	void EngineLogger::Destory()
	{
		if (!m_is_initialized)
			return;
		m_is_initialized = false;
		m_spd_logger->info("Destory Engine Logger");
	}

	void EngineLogger::SetLevel(LogLevel level)
	{
		m_log_level = level;
		if (m_is_initialized)
		{
			m_spd_logger->set_level(
				TypeTransform<spdlog::level::level_enum>(m_log_level));
		}

	}
}

