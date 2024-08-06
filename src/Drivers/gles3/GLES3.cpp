#include <gles3.h>

#include "Drivers/gles3/GLES3.h"

namespace Tabby {

GLES3* GLES3::s_Instance = nullptr;
GladGLES2Context* GLES3::gl = nullptr;

GLES3::GLES3()
{
    TB_CORE_ASSERT_TAGGED(!s_Instance, "GLES3 context already exists!");
    s_Instance = this;
}

void GLES3::Init(GladGLES2Context* context)
{
    if (!s_Instance) {
        s_Instance = new GLES3();
    }

    s_Instance->gl = context;
}
}
