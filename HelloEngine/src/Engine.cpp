#include "Engine/Engine.h"
#include <iostream>
//#include <vulkan/vulkan.h>
#include "core/EngineLogger.h"

namespace HelloEngine
{

HelloEngine::HelloEngine() : m_version("1.0.0")
{


}

HelloEngine::~HelloEngine(){}

void HelloEngine::Initialize()
{
    //LogInfo(" my LOGINFOR");
    Core::EngineLogger::GetInstance()->Info("my LOGINFOR");
 
    std::cout << "Engine initialized!!"<< std::endl;
}

void HelloEngine::Run()
{
    std::cout << "Engine running!!"<< std::endl;
}
    
void HelloEngine::ShutDown()
{
    std::cout << "Engine shutting down!!"<< std::endl;
}
const std::string& HelloEngine::GetVersin() const
{
    return m_version;
}

} // namespace HelloEngine
