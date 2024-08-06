#include "Tabby/Renderer/RenderCommand.h"
#include "tbpch.h"

namespace Tabby {

Scope<RendererAPI> RenderCommand::s_RendererAPI = RendererAPI::Create();

}
