#include "SandBox/SandBox.h"
#include <iostream>

namespace SandBox
{
SandBox::SandBox(){};

SandBox::~SandBox(){};

void SandBox::Start()
{
   m_engine.Initialize(); 
   m_engine.Run();
   std::cout << " version : " << m_engine.GetVersin() <<std::endl;
   m_engine.ShutDown();

}

    
} // namespace SandBox
