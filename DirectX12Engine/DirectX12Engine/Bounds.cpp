#include "Precompiled.h"
#include "Bounds.h"
#include "Mathf.h"

Bounds::Bounds(const Vector3& center, const Vector3& size)
{
    m_center = center;
    m_extents = size * 0.5f;
}

void Bounds::SetMinMax(const Vector3& min, const Vector3& max)
{
    m_extents = (max - min) * 0.5f;
    m_center = min + m_extents;
}

bool Bounds::Intersects(const Bounds& other) const
{
    Vector3 minA = GetMin();
    Vector3 maxA = GetMax();
    Vector3 minB = other.GetMin();
    Vector3 maxB = other.GetMax();

    return (minA.x <= maxB.x && maxA.x >= minB.x) &&
           (minA.y <= maxB.y && maxA.y >= minB.y) &&
           (minA.z <= maxB.z && maxA.z >= minB.z);
}

float Bounds::SqrDistance(const Vector3& point) const
{
    Vector3 closest = ClosestPoint(point);
    return (closest - point).SqrMagnitude();
}

bool Bounds::operator==(const Bounds& other)
{
    return m_center == other.m_center && m_extents == other.m_extents;
}

bool Bounds::operator!=(const Bounds& other)
{
    return !(*this == other);
}

Vector3 Bounds::ClosestPoint(const Vector3& point) const
{
    Vector3 min = GetMin();
    Vector3 max = GetMax();

    Vector3 result;
    result.x = Mathf::Clamp(point.x, min.x, max.x);
    result.y = Mathf::Clamp(point.y, min.y, max.y);
    result.z = Mathf::Clamp(point.z, min.z, max.z);
    
    return result;
}

bool Bounds::Contains(const Vector3& point) const
{
    Vector3 min = GetMin();
    Vector3 max = GetMax();

    return point.x >= min.x && point.x <= max.x &&
           point.y >= min.y && point.y <= max.y &&
           point.z >= min.z && point.z <= max.z;
}
