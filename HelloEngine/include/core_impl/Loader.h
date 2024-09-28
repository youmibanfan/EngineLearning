#pragma once

#include <vector>
#include <string>

namespace Core
{
	std::vector<std::string> EnumerateDevices();
	APIManager& Initialize();
	void Destroy();


}

