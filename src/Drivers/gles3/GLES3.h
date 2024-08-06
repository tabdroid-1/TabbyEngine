#pragma once

struct GladGLES2Context;

namespace Tabby {

class GLES3 {
public:
    GLES3();
    static void Init(GladGLES2Context*);
    static GladGLES2Context* GL() { return s_Instance->gl; }

private:
    static GladGLES2Context* gl;
    static GLES3* s_Instance;
};

}
