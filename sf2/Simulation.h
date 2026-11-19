#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "RungeKuttaSolver.h"
#include "Trajectory.h"

struct SimSettings
{
    float tiltDegrees = 15.0f;
    float omega0 = 40.0f;
    bool running = true;
    float simSpeed = 1.0f;
};

class Simulation
{
public:
    struct Energy
    {
        double kinetic = 0.0;
        double potential = 0.0;

        double Total() const
        {
            return kinetic + potential;
        }
    };

    Simulation();

    CubeParams& Params()
    {
        return m_Params;
    }
    const CubeParams& Params() const
    {
        return m_Params;
    }

    SimSettings& Settings()
    {
        return m_Settings;
    }
    const SimSettings& Settings() const
    {
        return m_Settings;
    }

    void Reset();
    void Advance(float dt);

    Energy ComputeEnergy() const;

    // Przez wartosc, nie przez referencje: stan zyje w double, a renderer i ImGui pracuja na float.
    glm::vec3 AngularVelocity() const
    {
        return glm::vec3(m_W);
    }
    glm::quat Orientation() const
    {
        return glm::quat(m_Q);
    }

    glm::vec3 FreeEndWorldPos() const;

    Trajectory& Trail()
    {
        return m_Trail;
    }
    const Trajectory& Trail() const
    {
        return m_Trail;
    }

private:
    static constexpr double kStep = 1e-3;
    static constexpr double kMaxFrameTime = 0.1;
    static constexpr int kSampleEveryNSteps = 10;

    CubeParams m_Params;
    SimSettings m_Settings;
    RungeKuttaSolver m_Solver{m_Params};

    glm::dvec3 m_W{0.0};
    glm::dquat m_Q{1.0, 0.0, 0.0, 0.0};

    // Takze double: ~167 odejmowan kStep na klatke od liczby rzedu 1e-2 gubilo we float
    // kilka dziesiatych procenta kroku na klatke, systematycznie w jedna strone.
    double m_Accumulator = 0.0;
    int m_StepsSinceSample = 0;

    Trajectory m_Trail;
};
