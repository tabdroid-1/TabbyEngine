#pragma once

// For use by Tabby applications
//
// --- Random -----------------------
#include <Tabby/Math/Math.h>
#include <Tabby/Math/Random.h>
// --- Core -------------------------
#include <Tabby/Core/Base.h>
#include <Tabby/Core/Assert.h>
#include <Tabby/Core/Log/Log.h>
#include <Tabby/Core/Layer/Layer.h>
#include <Tabby/Core/Application.h>
#include <Tabby/Core/Time/Time.h>
#include <Tabby/Core/Time/Timer.h>
// -----------------------------------
//
// --- Scene -------------------------
#include <Tabby/World/World.h>
#include <Tabby/World/Entity.h>
#include <Tabby/World/Components.h>
// -----------------------------------
//
// --- Physics -----------------------
#include <Tabby/Physics/2D/Physics2D.h>
#include <Tabby/Physics/2D/Physics2DTypes.h>
// -----------------------------------
//
// --- Input -------------------------
#include <Tabby/Core/Input/Input.h>
#include <Tabby/Core/Input/KeyCodes.h>
#include <Tabby/Core/Input/MouseCodes.h>
// -----------------------------------
//
// --- Asset -------------------------
#include <Tabby/Asset/AssetManager.h>
#include <Tabby/Asset/GLTFLoader.h>
// -----------------------------------
//
// --- Renderer ----------------------
#include <Tabby/Renderer/Mesh.h>
#include <Tabby/Renderer/Buffer.h>
#include <Tabby/Renderer/Shader.h>
#include <Tabby/Renderer/Texture.h>
#include <Tabby/Renderer/Renderer.h>
#include <Tabby/Renderer/Material.h>
#include <Tabby/Renderer/Renderer2D.h>
#include <Tabby/Renderer/VertexArray.h>
#include <Tabby/Renderer/Framebuffer.h>
#include <Tabby/Renderer/RenderCommand.h>
// -----------------------------------
//
// --- Debug -------------------------
#include <Tabby/Debug/Debug.h>
#include <Tabby/Debug/Instrumentor.h>
// -----------------------------------
//
// --- UI ----------------------------
#include <Tabby/UI/ImGui/ImGuiLayer.h>
// -----------------------------------
//
// --- Util --------------------------
#include <Tabby/Utils/Utils.h>
#include <Tabby/Utils/PlatformUtils.h>
// -----------------------------------
