#pragma once

#include "Tabby/Core/PlatformDetection.h"

#ifdef TB_PLATFORM_WINDOWS
#ifndef NOMINMAX
// See github.com/skypjack/entt/wiki/Frequently-Asked-Questions#warning-c4003-the-min-the-max-and-the-macro
#define NOMINMAX
#endif
#endif

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <random>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "Tabby/Core/Assert.h"
#include "Tabby/Core/Base.h"
#include "Tabby/Core/Buffer.h"
#include "Tabby/Core/Log.h"
#include "Tabby/Debug/Instrumentor.h"

#ifdef TB_PLATFORM_WINDOWS
#include <Windows.h>
#endif
