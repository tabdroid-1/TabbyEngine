#pragma once

#include "Tabby/Core/PlatformDetection.h"

#include <memory>

#if defined(TB_PLATFORM_WINDOWS)
#define TB_DEBUGBREAK() __debugbreak()
#elif defined(TB_PLATFORM_LINUX)
#include <signal.h>
#define TB_DEBUGBREAK() raise(SIGTRAP)
#elif defined(TB_PLATFORM_MACOS)
#include <signal.h>
#define TB_DEBUGBREAK() raise(SIGTRAP)
#elif defined(TB_PLATFORM_ANDROID)
#define TB_DEBUGBREAK() __builtin_trap()
#elif defined(TB_PLATFORM_WEB)
#include <emscripten.h>
#define TB_DEBUGBREAK() emscripten_force_exit(1)
// #define TB_DEBUGBREAK() emscripten_debugger()
#else
#error "Platform doesn't support debugbreak yet!"
#endif

#ifdef TB_DEBUG
#define TB_ENABLE_ASSERTS
#endif

#ifndef TB_DIST
#define TB_ENABLE_VERIFY
#endif

#define TB_EXPAND_MACRO(x) x
#define TB_STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << x)

#define TB_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace Tabby {

template <typename T>
using Scope = std::unique_ptr<T>;
template <typename T, typename... Args>
constexpr Scope<T> CreateScope(Args&&... args)
{
    return std::make_unique<T>(std::forward<Args>(args)...);
}

template <typename T>
using Shared = std::shared_ptr<T>;
template <typename T, typename... Args>
constexpr Shared<T> CreateShared(Args&&... args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}

template <typename T1, typename T2>
Shared<T1> ShareAs(Shared<T2>& ptr)
{
    return std::static_pointer_cast<T1>(ptr);
};

template <typename T1, typename T2>
Shared<T1> ShareAs(const Shared<T2>& ptr)
{
    return std::static_pointer_cast<T1>(ptr);
};

}

#include "Tabby/Core/Assert.h"
#include "Tabby/Core/Log/Log.h"
