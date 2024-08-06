#ifdef TB_PLATFORM_WEB

#include "Tabby/Core/Application.h"
#include "Tabby/Utils/PlatformUtils.h"
#include "Tabby/Core/Time/Time.h"
#include "tbpch.h"

#include <SDL.h>

namespace Tabby {

double Time::GetTime()
{
    using namespace std::chrono;
    system_clock::time_point currentTimePoint = system_clock::now();
    duration<double> timeSinceEpoch = currentTimePoint.time_since_epoch();

    double result = timeSinceEpoch.count();

    return result;
}

std::string FileDialogs::OpenFile(const char* filter)
{
    return std::string();
}

std::string FileDialogs::SaveFile(const char* filter)
{

    return std::string();
}

}

#endif
