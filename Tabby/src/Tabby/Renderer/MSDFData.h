#pragma once

#if !defined TB_PLATFORM_WEB && !defined TB_PLATFORM_ANDROID
#include <vector>

#undef INFINITE
#include "msdf-atlas-gen.h"

namespace Tabby {

struct MSDFData {
    std::vector<msdf_atlas::GlyphGeometry> Glyphs;
    msdf_atlas::FontGeometry FontGeometry;
};

}
#endif
