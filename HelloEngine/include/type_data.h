#pragma once

//#include<vulkan/vulkan_enums.hpp> 
#include <cstdint>
#include <typeinfo> // to learn

namespace Core
{
	enum class LogLevel
	{
		eTrace,
		eDebug,
		eInfo,
		eError,
		eWarn,
		eFatal
	};
    struct WindowSize 
	{
        uint32_t m_width;
        uint32_t m_height;
    };

    using ClassHashCode = decltype(typeid(int).hash_code());
}
