#pragma once

#undef INFINITE
#include "msdf-atlas-gen.h"

namespace Tabby {

struct MSDFData {
    std::vector<msdf_atlas::GlyphGeometry> Glyphs;
    msdf_atlas::FontGeometry FontGeometry;
};

}
