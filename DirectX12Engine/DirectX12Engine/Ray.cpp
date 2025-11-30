#include "Precompiled.h"
#include "Ray.h"

Ray::Ray(const Vector3& origin, const Vector3& direction)
    : m_origin(origin)
    , m_direction(direction)
{

}

Vector3 Ray::GetPoint(float distance) const
{
    return m_origin + m_direction * distance;
}
