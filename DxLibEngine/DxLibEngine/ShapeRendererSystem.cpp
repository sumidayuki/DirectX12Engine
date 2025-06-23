#include "ShapeRendererSystem.h"

void ShapeRendererSystem::Draw(ComponentManager& cm, World& world)
{
	View<Box, Transform> boxView(cm);

	// squares‚ğentity‚Æsquare‚É•ª‰ğ‚µ‚Ä’Tõ‚µ‚Ü‚·B
	for (auto [entity, box, transform] : boxView)
	{
		if (!box.isActive) continue;
		DrawBox(transform.position.x, transform.position.y, transform.position.x + (int)box.x, transform.position.y + (int)box.y, GetColor(box.r, box.g, box.b), TRUE);
	}

	View<Circle, Transform> circleView(cm);

	// circles‚ğentity‚Æcircle‚É•ª‰ğ‚µ‚Ä’Tõ‚µ‚Ü‚·B
	for (auto [entity, circle, transform] : circleView)
	{
		if (!circle.isActive) continue;
		DrawCircle(transform.position.x, transform.position.y, circle.radius, GetColor(circle.r, circle.g, circle.b), TRUE);
	}

	View<Triangle, Transform> triangleView(cm);

	// triangles‚ğentity‚Ætriangle‚É•ª‰ğ‚µ‚Ä’Tõ‚µ‚Ü‚·B
	for (auto [entity, triangle, transform] : triangleView)
	{
		if (triangle.isActive) continue;
		DrawTriangle
		(
			(int)triangle.x1 + transform.position.x, (int)triangle.y1 + transform.position.y,
			(int)triangle.x2 + transform.position.x, (int)triangle.y2 + transform.position.y,
			(int)triangle.x3 + transform.position.x, (int)triangle.y3 + transform.position.y,
			GetColor(triangle.r, triangle.g, triangle.b),
			TRUE
		);
	}
}
