#pragma once

#if !TB_PROFILE || defined(TB_PLATFORM_ANDROID)

#define TB_PROFILE_SCOPE()
#define TB_PROFILE_SCOPE_NAME(x)
#define TB_PROFILE_FRAME(x)
#define TB_PROFILE_TAG(y, x)
#define TB_PROFILE_LOG(text, size)
#define TB_PROFILE_VALUE(text, value)
#define TB_PROFILE_ALLOC(p, size)
#define TB_PROFILE_FREE(p)
#define TB_PROFILE_GPU(x)

#elif TB_PROFILE && !defined(TB_PLATFORM_ANDROID)
#define TRACY_ENABLE 1
#include "tracy/tracy/Tracy.hpp"
#include "tracy/tracy/TracyC.h"
#include "tracy/tracy/TracyOpenGL.hpp"

#define TB_PROFILE_SCOPE() ZoneScoped
#define TB_PROFILE_SCOPE_NAME(x) ZoneScopedN(x)
#define TB_PROFILE_FRAME(x) FrameMark
#define TB_PROFILE_TAG(y, x) ZoneText(x, strlen(x))
#define TB_PROFILE_LOG(text, size) TracyMessage(text, size)
#define TB_PROFILE_VALUE(text, value) TracyPlot(text, value)
#define TB_PROFILE_ALLOC(p, size) TracyCAllocS(p, size, 12)
#define TB_PROFILE_FREE(p) TracyCFreeS(p, 12)
#define TB_PROFILE_GPU(x) TracyGpuZone(x)

#endif
