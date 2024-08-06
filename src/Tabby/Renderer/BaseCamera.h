#pragma once

#include <glm/glm.hpp>

namespace Tabby {

class BaseCamera {
public:
    BaseCamera() = default;
    BaseCamera(const glm::mat4& projection)
        : m_Projection(projection)
    {
    }

    virtual ~BaseCamera() = default;

    const glm::mat4& GetProjection() const { return m_Projection; }

protected:
    glm::mat4 m_Projection = glm::mat4(1.0f);
};

}
