#include "Bounds.h"

Bounds::Bounds(const Vector3& center, const Vector3& size)
    : m_center(center)
    , m_extents(size * 0.5f)
{

}


void Bounds::SetMinMax(const Vector3& min, const Vector3& max)
{
    m_extents = (max + min) * 0.5f;
    m_center = min + m_extents;
}


Vector3 Bounds::ClosestPoint(const Vector3& point) const
{
    return Vector3();
}


bool Bounds::Contains(const Vector3& point) const
{
    if (Mathf::Abs(point.x - m_center.x) > m_extents.x)
        return false;

    if (Mathf::Abs(point.y - m_center.y) > m_extents.y)
        return false;

    if (Mathf::Abs(point.z - m_center.z) > m_extents.z)
        return false;

    return true;
}

void Bounds::Expand(float amount)
{
    amount *= 0.5f;
    m_extents.x += amount;
    m_extents.y += amount;
    m_extents.z += amount;
}

float Bounds::SqrDistance(const Vector3& point) const
{
    assert(!"Unsupported");
    return 0.0f;
}

bool Bounds::Intersects(const Bounds& other) const
{
    if (Mathf::Abs(m_center.x - other.m_center.x) > (m_extents.x + other.m_extents.x))
        return false;

    if (Mathf::Abs(m_center.y - other.m_center.y) > (m_extents.y + other.m_extents.y))
        return false;

    if (Mathf::Abs(m_center.z - other.m_center.z) > (m_extents.z + other.m_extents.z))
        return false;

    return true;
}


bool Bounds::operator==(const Bounds& other)
{
    return (m_center == other.m_center) && (m_extents == other.m_extents);
}


bool Bounds::operator!=(const Bounds& other)
{
    return !operator==(other);
}

