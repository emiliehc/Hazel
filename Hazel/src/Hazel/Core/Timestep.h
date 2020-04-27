#pragma once

namespace Hazel
{
    class Timestep
    {
    public:
        Timestep(float time) : m_Time(time)
        {
        }

        float GetSeconds() const
        {
            return m_Time;
        }

        float GetMilliseconds() const
        {
            return m_Time * 1000;
        }

        operator float() const
        {
            return m_Time;
        }

    private:
        float m_Time;
    };
}
