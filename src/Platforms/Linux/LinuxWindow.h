#pragma once

#ifdef TB_PLATFORM_LINUX

#include "Tabby/Core/Window.h"
#include "Tabby/Renderer/GraphicsContext.h"

class SDL_Window;

namespace Tabby {

class LinuxWindow : public Window {
public:
    LinuxWindow(const WindowProps& props);
    virtual ~LinuxWindow();

    void OnUpdate() override;

    unsigned int GetWidth() const override { return m_Data.Width; }
    unsigned int GetHeight() const override { return m_Data.Height; }

    // Window attributes
    void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }

    void SetVSync(bool enabled) override;
    bool IsVSync() const override;

    void SetResizable(bool enabled) override;
    bool GetResizable() const override;

    void SetMinSize(uint32_t minWidth, uint32_t minHeight) override;

    virtual void* GetNativeWindow() const { return m_Window; }

private:
    virtual void Init(const WindowProps& props);
    virtual void Shutdown();

private:
    SDL_Window* m_Window;
    Scope<GraphicsContext> m_Context;

    struct WindowData {
        std::string Title;
        unsigned int Width, Height;
        unsigned int MinWidth, MinHeight;
        bool Resizeable;
        bool VSync;

        EventCallbackFn EventCallback;
    };

    WindowData m_Data;
};

}

#endif
