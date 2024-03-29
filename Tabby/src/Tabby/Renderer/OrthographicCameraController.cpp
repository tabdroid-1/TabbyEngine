#include "Tabby/Renderer/OrthographicCameraController.h"
#include "tbpch.h"

#include "Tabby/Core/Input.h"
#include "Tabby/Core/KeyCodes.h"

namespace Tabby {

OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation)
    : m_AspectRatio(aspectRatio)
    , m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel)
    , m_Rotation(rotation)
{
}

void OrthographicCameraController::OnUpdate(Timestep ts)
{
    TB_PROFILE_SCOPE();

    if (Input::IsKeyPressed(Key::A)) {
        m_CameraPosition.x -= cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
        m_CameraPosition.y -= sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
    } else if (Input::IsKeyPressed(Key::D)) {
        m_CameraPosition.x += cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
        m_CameraPosition.y += sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
    }

    if (Input::IsKeyPressed(Key::W)) {
        m_CameraPosition.x += -sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
        m_CameraPosition.y += cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
    } else if (Input::IsKeyPressed(Key::S)) {
        m_CameraPosition.x -= -sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
        m_CameraPosition.y -= cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
    }

    if (m_Rotation) {
        if (Input::IsKeyPressed(Key::Q))
            m_CameraRotation += m_CameraRotationSpeed * ts;
        if (Input::IsKeyPressed(Key::E))
            m_CameraRotation -= m_CameraRotationSpeed * ts;

        if (m_CameraRotation > 180.0f)
            m_CameraRotation -= 360.0f;
        else if (m_CameraRotation <= -180.0f)
            m_CameraRotation += 360.0f;

        m_Camera.SetRotation(m_CameraRotation);
    }

    m_Camera.SetPosition(m_CameraPosition);

    m_CameraTranslationSpeed = m_ZoomLevel;
}

void OrthographicCameraController::OnEvent(Event& e)
{
    TB_PROFILE_SCOPE();

    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<MouseScrolledEvent>(TB_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
    dispatcher.Dispatch<WindowResizeEvent>(TB_BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));
}

void OrthographicCameraController::OnResize(float width, float height)
{
    m_AspectRatio = width / height;
    m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
}

bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e)
{
    TB_PROFILE_SCOPE();

    m_ZoomLevel -= e.GetYOffset() * 0.25f;
    m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
    m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
    return false;
}

bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& e)
{
    TB_PROFILE_SCOPE();

    OnResize((float)e.GetWidth(), (float)e.GetHeight());
    return false;
}

}
