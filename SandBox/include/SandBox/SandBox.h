#pragma once 
#include "Engine/Engine.h"
namespace SandBox
{
class SandBox
{
public:
    SandBox();
    ~SandBox();

    void Start();

private:
    HelloEngine::HelloEngine m_engine;
};
    
} // namespace SandBox
