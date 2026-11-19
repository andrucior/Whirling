#pragma once

#include <tuple>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct CubeParams
{
    float a = 1.f;
    float density = 1.f;
    bool gravityOn = true;
    glm::vec3 g{0.f, -9.81f, 0.f};
};

class RungeKuttaSolver
{
public:
    explicit RungeKuttaSolver(const CubeParams& params) : p(params)
    {
        updateDerived();
    }

    std::tuple<glm::dvec3, glm::dquat> nextStep(glm::dvec3 W, glm::dquat Q, double h)
    {
        if (p.a != cachedA || p.density != cachedDensity)
            updateDerived();

        auto [k1W, k1Q] = f(W, Q);
        auto [k2W, k2Q] = f(W + k1W * h * 0.5, Q + k1Q * (h * 0.5));
        auto [k3W, k3Q] = f(W + k2W * h * 0.5, Q + k2Q * (h * 0.5));
        auto [k4W, k4Q] = f(W + k3W * h, Q + k3Q * h);

        W = W + h / 6.0 * (k1W + 2.0 * k2W + 2.0 * k3W + k4W);
        Q = Q + h / 6.0 * (k1Q + 2.0 * k2Q + 2.0 * k3Q + k4Q);

        return {W, glm::normalize(Q)};
    }

    double mass() const
    {
        return m;
    }
    const glm::dmat3& inertia() const
    {
        return I;
    }

private:
    const CubeParams& p;

    float cachedA = -1.f, cachedDensity = -1.f;
    double m = 1.0;
    glm::dmat3 I{1.0}, I_inv{1.0};
    glm::dvec3 r_c{0.5};

    void updateDerived()
    {
        const double a = p.a;
        m = static_cast<double>(p.density) * a * a * a;

        const double d = 2.0 / 3.0, o = -1.0 / 4.0;
        I = m * a * a * glm::dmat3(
            d, o, o,
            o, d, o,
            o, o, d
        );

        const double k = 1.0 / (11.0 * m * a * a);

        I_inv = k * glm::dmat3(
            30, 18, 18,
            18, 30, 18,
            18, 18, 30
        );

        r_c = glm::dvec3(0.5 * a);

        cachedA = p.a;
        cachedDensity = p.density;
    }

    std::tuple<glm::dvec3, glm::dquat> f(const glm::dvec3& W, const glm::dquat& Q) const
    {
        glm::dvec3 N(0.0);
        if (p.gravityOn)
        {
            const glm::dvec3 gBody = glm::conjugate(Q) * glm::dvec3(p.g);
            N = glm::cross(r_c, m * gBody);
        }

        const glm::dvec3 dW = I_inv * (N + glm::cross(I * W, W));
        const glm::dquat dQ = 0.5 * (Q * glm::dquat(0.0, W));
        return {dW, dQ};
    }
};
