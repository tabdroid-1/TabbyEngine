#include "Tabby/Core/Window.h"
#include "tbpch.h"

#ifdef TB_PLATFORM_WINDOWS
#include "Platforms/Windows/WindowsWindow.h"
#elif defined(TB_PLATFORM_LINUX)
#include "Platforms/Linux/LinuxWindow.h"
#elif defined(TB_PLATFORM_MACOS)
#include "Platforms/MacOS/MacOSWindow.h"
#endif

namespace Tabby {
Scope<Window> Window::Create(const WindowProps& props)
{
#ifdef TB_PLATFORM_WINDOWS
    return CreateScope<WindowsWindow>(props);
#elif defined(TB_PLATFORM_LINUX)
    return CreateScope<LinuxWindow>(props);
#elif defined(TB_PLATFORM_MACOS)
    return CreateScope<MacOSWindow>(props);
#else
    TB_CORE_ASSERT(false, "Unknown platform!");
    return nullptr;
#endif
}

}
