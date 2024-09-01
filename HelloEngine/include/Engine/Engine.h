#pragma once

#include <string>
#include <HelloEngineExport.h>

namespace HelloEngine
{

class HELLO_ENGINE_API HelloEngine
{
public:
    HelloEngine();
    ~HelloEngine();

    void Initialize();
    void Run();
    void ShutDown();
    const std::string& GetVersin() const;

private:
    std::string m_version;
};


}// namespace Engine