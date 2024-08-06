#pragma once

#include "Tabby/Core/Base.h"
#include "Tabby/Core/Log/Log.h"

#define TB_CORE_VERIFY(expression)                                           \
    do {                                                                     \
        if (!(expression)) {                                                 \
            TB_CORE_ERROR("Assertion failed: {0}({1})", __FILE__, __LINE__); \
            TB_DEBUGBREAK();                                                 \
        }                                                                    \
    } while (false)
#define TB_CORE_VERIFY_TAGGED(expression, ...)      \
    do {                                            \
        if (!(expression)) {                        \
            TB_CORE_ERROR(                          \
                "Assertion failed: {0}({1}) | {2}", \
                __FILE__,                           \
                __LINE__,                           \
                __VA_ARGS__);                       \
            TB_DEBUGBREAK();                        \
        }                                           \
    } while (false)

#ifdef TB_ENABLE_ASSERTS

#define TB_CORE_ASSERT(expression)                                           \
    do {                                                                     \
        if (!(expression)) {                                                 \
            TB_CORE_ERROR("Assertion failed: {0}({1})", __FILE__, __LINE__); \
            TB_DEBUGBREAK();                                                 \
        }                                                                    \
    } while (false)
#define TB_CORE_ASSERT_TAGGED(expression, ...)      \
    do {                                            \
        if (!(expression)) {                        \
            TB_CORE_ERROR(                          \
                "Assertion failed: {0}({1}) | {2}", \
                __FILE__,                           \
                __LINE__,                           \
                __VA_ARGS__);                       \
            TB_DEBUGBREAK();                        \
        }                                           \
    } while (false)
#else
#define TB_CORE_ASSERT(expression)
#define TB_CORE_ASSERT_TAGGED(expression, ...)
#endif
