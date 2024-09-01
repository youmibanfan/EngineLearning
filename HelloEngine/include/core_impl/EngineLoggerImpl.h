#pragma once

#include <spdlog/spdlog.h>
#include "Common.h"
#include "type_data.h"
//#include <spdlog/sinks/stdout_color_sinks.h>
//#include "HelloEngineExport.h"

namespace Core
{
class EngineLogger
{
	default_no_copy_move_construction(EngineLogger)
public:
	static EngineLogger* GetInstance();
	static void SetLogLevel(LogLevel level);
	
public:

	void Initialize();
	void Destory();
	void SetLevel(LogLevel level);


	template<typename... Args>
	void Trace(Args &&... args)
	{
		m_spd_logger->trace(std::forward<Args>(args) ...);
	}

	template<typename... Args>
	void Debug(Args &&... args)
	{
		m_spd_logger->debug(std::forward<Args>(args) ...);
	}

	template<typename... Args>
	void Info(Args &&... args)
	{
		m_spd_logger->info(std::forward<Args>(args) ...);
	}

	template<typename... Args>
	void Warn(Args &&... args)
	{
		m_spd_logger->warn(std::forward<Args>(args) ...);
	}

	template<typename... Args>
	void Error(Args &&... args)
	{
		m_spd_logger->error(std::forward<Args>(args) ...);
	}

	template<typename... Args>
	void Fatal(Args &&... args)
	{
		m_spd_logger->critical(std::forward<Args>(args) ...);
	}

private:
	bool m_is_initialized{ false };
	std::shared_ptr<spdlog::logger> m_spd_logger;
	LogLevel m_log_level = LogLevel::eInfo;
};

#define LogTrace(...)	::Core::EngineLogger::GetInstance()->Trace(__VA_ARGS__);
#define LogDebug(...)	::Core::EngineLogger::GetInstance()->Debug(__VA_ARGS__);
#define LogInfo(...)	::Core::EngineLogger::GetInstance()->Info(__VA_ARGS__);
#define LogWarn(...)	::Core::EngineLogger::GetInstance()->Warn(__VA_ARGS__);
#define LogError(...)	::Core::EngineLogger::GetInstance()->Error(__VA_ARGS__);
#define LogFatal(...)	::Core::EngineLogger::GetInstance()->Fatal(__VA_ARGS__);




}
