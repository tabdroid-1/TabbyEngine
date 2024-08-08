#include <Tabby/Core/Log/Log.h>

#include <Tabby/Foundation/Types.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#if defined(TB_PLATFORM_ANDROID)
#include "spdlog/sinks/android_sink.h"
#endif

namespace Tabby {

Shared<spdlog::logger> Log::s_CoreLogger;
Shared<spdlog::logger> Log::s_ClientLogger;

void Log::Init()
{
    std::vector<spdlog::sink_ptr> logSinks;
#if defined(TB_PLATFORM_ANDROID)
    logSinks.emplace_back(std::make_shared<spdlog::sinks::android_sink_mt>());
    logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
#else
    logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("Tabby.log", true));
#endif

    logSinks[0]->set_pattern("%^[%T] %n: %v%$");
    logSinks[1]->set_pattern("[%T] [%l] %n: %v");

    s_CoreLogger = std::make_shared<spdlog::logger>("Tabby", begin(logSinks), end(logSinks));
    spdlog::register_logger(s_CoreLogger);
    s_CoreLogger->set_level(spdlog::level::trace);
    s_CoreLogger->flush_on(spdlog::level::trace);

    s_ClientLogger = std::make_shared<spdlog::logger>("APP", begin(logSinks), end(logSinks));
    spdlog::register_logger(s_ClientLogger);
    s_ClientLogger->set_level(spdlog::level::trace);
    s_ClientLogger->flush_on(spdlog::level::trace);
}
}

// auto fmt::formatter<glm::vec<2, int>>::format(glm::vec<2, int> c, format_context& ctx) const -> format_context::iterator
// {
//     string_view name = fmt::format("X: {}  Y: {}", c.x, c.y);
//     return formatter<string_view>::format(name, ctx);
// }
//
// auto fmt::formatter<glm::vec<3, int>>::format(glm::vec<3, int> c, format_context& ctx) const -> format_context::iterator
// {
//     string_view name = fmt::format("X: {}  Y: {}  Z: {}", c.x, c.y, c.z);
//     return formatter<string_view>::format(name, ctx);
// }
//
// auto fmt::formatter<glm::vec<4, int>>::format(glm::vec<4, int> c, format_context& ctx) const -> format_context::iterator
// {
//     string_view name = fmt::format("X: {}  Y: {}  Z: {}  W: {}", c.x, c.y, c.z, c.w);
//     return formatter<string_view>::format(name, ctx);
// }
//
// auto fmt::formatter<glm::vec<2, uint32_t>>::format(glm::vec<2, uint32_t> c, format_context& ctx) const -> format_context::iterator
// {
//     string_view name = fmt::format("X: {}  Y: {}", c.x, c.y);
//     return formatter<string_view>::format(name, ctx);
// }
//
// auto fmt::formatter<glm::vec<3, uint32_t>>::format(glm::vec<3, uint32_t> c, format_context& ctx) const -> format_context::iterator
// {
//     string_view name = fmt::format("X: {}  Y: {}  Z: {}", c.x, c.y, c.z);
//     return formatter<string_view>::format(name, ctx);
// }
//
// auto fmt::formatter<glm::vec<4, uint32_t>>::format(glm::vec<4, uint32_t> c, format_context& ctx) const -> format_context::iterator
// {
//     string_view name = fmt::format("X: {}  Y: {}  Z: {}  W: {}", c.x, c.y, c.z, c.w);
//     return formatter<string_view>::format(name, ctx);
// }
//
// auto fmt::formatter<glm::vec2>::format(glm::vec2 c, format_context& ctx) const -> format_context::iterator
// {
//     string_view name = fmt::format("X: {}  Y: {}", c.x, c.y);
//     return formatter<string_view>::format(name, ctx);
// }
//
// auto fmt::formatter<glm::vec3>::format(glm::vec3 c, format_context& ctx) const -> format_context::iterator
// {
//     string_view name = fmt::format("X: {}  Y: {}  Z: {}", c.x, c.y, c.z);
//     return formatter<string_view>::format(name, ctx);
// }
//
// auto fmt::formatter<glm::vec4>::format(glm::vec4 c, format_context& ctx) const -> format_context::iterator
// {
//     string_view name = fmt::format("X: {}  Y: {}  Z: {}  W: {}", c.x, c.y, c.z, c.w);
//     return formatter<string_view>::format(name, ctx);
// }
//
// auto fmt::formatter<glm::quat>::format(glm::quat c, format_context& ctx) const -> format_context::iterator
// {
//     string_view name = fmt::format("X: {}  Y: {}  Z: {}  W: {}", c.x, c.y, c.z, c.w);
//     return formatter<string_view>::format(name, ctx);
// }
