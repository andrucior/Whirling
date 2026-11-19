#include "Simulation.h"

#include <algorithm>
#include <tuple>

#include <glm/gtc/matrix_transform.hpp>

constexpr double kInvSqrt3 = 0.57735026918962576;
constexpr glm::dvec3 kBodyDiagonal(kInvSqrt3, kInvSqrt3, kInvSqrt3);
constexpr glm::dvec3 kWorldUp(0.0, 1.0, 0.0);
constexpr glm::dvec3 kTiltAxis(0.0, 0.0, 1.0);


Simulation::Simulation()
{
    Reset();
}

void Simulation::Reset()
{
    const glm::dquat standOnCorner(kBodyDiagonal, kWorldUp);
    const glm::dquat tilt = glm::angleAxis(glm::radians(static_cast<double>(m_Settings.tiltDegrees)), kTiltAxis);

    m_Q = glm::normalize(tilt * standOnCorner);
    m_W = static_cast<double>(m_Settings.omega0) * kBodyDiagonal;
    m_Accumulator = 0.0;
    m_StepsSinceSample = 0;
    m_Trail.Reset();
}

void Simulation::Advance(float dt)
{
    if (!m_Settings.running)
    {
        return;
    }

    m_Accumulator += std::min(static_cast<double>(dt), kMaxFrameTime) * m_Settings.simSpeed;

    while (m_Accumulator >= kStep)
    {
        std::tie(m_W, m_Q) = m_Solver.nextStep(m_W, m_Q, kStep);
        m_Accumulator -= kStep;

        if (++m_StepsSinceSample >= kSampleEveryNSteps)
        {
            m_StepsSinceSample = 0;
            m_Trail.Add(FreeEndWorldPos());
        }
    }
}

Simulation::Energy Simulation::ComputeEnergy() const
{
    // Energia calkowita powinna byc ~stala
    const glm::dvec3 centerOfMass = m_Q * glm::dvec3(0.5 * m_Params.a);

    Energy energy;
    energy.kinetic = 0.5 * glm::dot(m_W, m_Solver.inertia() * m_W);
    energy.potential =
        m_Params.gravityOn ? -m_Solver.mass() * glm::dot(glm::dvec3(m_Params.g), centerOfMass) : 0.0;
    return energy;
}

glm::vec3 Simulation::FreeEndWorldPos() const
{
    return glm::vec3(m_Q * glm::dvec3(m_Params.a));
}
