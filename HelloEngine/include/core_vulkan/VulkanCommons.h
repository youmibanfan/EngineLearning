#pragma once

#include <Common.h>
#include <vulkan/vulkan.hpp>
#include "vk_mem_alloc.h"
#include "core_impl/EngineLoggerImpl.h"
#include "vulkan/vulkan_enums.hpp"

#define __vk_test(result, Level)	\
if((result)!= vk::Result::eSuccess)	\
	Log##Level("File: {} Line {} Func: {} Vulkan Check Failed", __FILE__, __LINE__, __FUNCTION__);

#define vk_check(result) __vk_test(result, Error)
#define vk_assert(result) __vk_test(result, Fatal)
