#pragma once

class Ray
{
private:
	Vector3 m_origin;		// n“_
	Vector3 m_direction;	// Œü‚«(³‹K‰»‚³‚ê‚Ä‚¢‚é‚±‚Æ‚ğ‘z’è)

public:
	Ray(const Vector3& origin, const Vector3& direction);

	const Vector3& GetOrigin() const { return m_origin; }

	const Vector3& GetDirection() const { return m_direction; }

	Vector3 GetPoint(float distance) const;
};