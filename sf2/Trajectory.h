#pragma once

#include <algorithm>
#include <vector>
#include <glm/glm.hpp>
#include "Polyline.h"

class Trajectory : public Polyline
{
public:
    static constexpr size_t kCapacity = 10000;

    Trajectory()
    {
        m_Points.reserve(kCapacity);
        ReservePoints(kCapacity);
    }

    void Add(const glm::vec3& point)
    {
        if (m_Points.size() == kCapacity)
        {
            DropOldestHalf();
        }

        m_Points.push_back(point);
    }

    void Reset()
    {
        m_Points.clear();
        m_UploadedCount = 0;
    }

    size_t GetPointCount() const
    {
        return m_Points.size();
    }

    void Sync()
    {
        if (m_UploadedCount == m_Points.size())
        {
            return;
        }

        UploadPoints(m_Points.data(), m_Points.size(), m_UploadedCount);
        m_UploadedCount = m_Points.size();
    }

private:
    void DropOldestHalf()
    {
        const size_t keep = kCapacity / 2;

        std::move(m_Points.end() - keep, m_Points.end(), m_Points.begin());
        m_Points.resize(keep);

        m_UploadedCount = 0;
    }

    std::vector<glm::vec3> m_Points;
    size_t m_UploadedCount = 0;
};
