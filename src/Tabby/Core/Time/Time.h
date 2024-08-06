#pragma once

namespace Tabby {

class Time {
public:
    static double GetTime();
    static double GetDeltaTime() { return m_DeltaTime; }

private:
    static void SetDeltaTime(double deltaTime) { m_DeltaTime = deltaTime; }

private:
    inline static double m_DeltaTime;

    friend class Application;
};

}
