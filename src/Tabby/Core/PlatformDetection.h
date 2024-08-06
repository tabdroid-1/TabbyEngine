// Platform detection using predefined macros
// TODO: Add TB_ARCH
#ifdef _WIN32
/* Windows x64/x86 */
#ifdef _WIN64
/* Windows x64  */
#define TB_PLATFORM_WINDOWS
#else
/* Windows x86 */
#define TB_PLATFORM_WINDOWS
#endif
#elif defined(__APPLE__) || defined(__MACH__)
#include <TargetConditionals.h>
/* TARGET_OS_MAC exists on all the platforms
 * so we must check all of them (in this order)
 * to ensure that we're running on MAC
 * and not some other Apple platform */
#if TARGET_IPHONE_SIMULATOR
#define TB_PLATFORM_IOS
#error "IOS simulator is not supported!"
#elif TARGET_OS_IPHONE
#define TB_PLATFORM_IOS
#error "IOS is not supported!"
#elif TARGET_OS_MAC
#define TB_PLATFORM_MACOS
#else
#error "Unknown Apple platform!"
#endif
/* We also have to check __ANDROID__ before __linux__
 * since android is based on the linux kernel
 * it has __linux__ defined */
#elif defined(__ANDROID__)
#define TB_PLATFORM_ANDROID
#elif defined(__linux__)
#define TB_PLATFORM_LINUX
#elif defined(__EMSCRIPTEN__)
#define TB_PLATFORM_WEB
#else
/* Unknown compiler/platform */
#error "Unknown platform!"
#endif // End of platform detection

#if defined(__x86_64__) || defined(_M_X64)
#define TB_CPU_X86_64
#elif defined(__i386__) || defined(_M_IX86)
#define TB_CPU_X86_32
#elif defined(__aarch64__) || defined(_M_ARM64)
#define TB_CPU_ARM64
#elif defined(__arm__) || defined(_M_ARM)
#define TB_CPU_ARM32
#elif defined(__EMSCRIPTEN__)
#define TB_CPU_WASM
#else
#error Unknown CPU architexture
#endif
