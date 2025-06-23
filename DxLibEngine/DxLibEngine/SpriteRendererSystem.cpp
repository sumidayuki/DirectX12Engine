#include "SpriteRendererSystem.h"

void SpriteRendererSystem::Draw(ComponentManager& cm, World& world)
{
	View<Sprite, Transform> view(cm);

	// spritesÇentityÇ∆spriteÇ…ï™âÇµÇƒíTçıÇµÇ‹Ç∑ÅB
	for (auto [entity, sprite, transform] : view)
	{
		if (!entity.enabled) continue;

		float w = sprite.width;
		float h = sprite.height;

		float pivotX = sprite.pivot;
		float pivotY = sprite.pivot;

		Vector3 localLT = Vector3(-w * pivotX, -h * pivotY, 0);
		Vector3 localRT = Vector3(w * (1.0f - pivotX), -h * pivotY, 0);
		Vector3 localLB = Vector3(-w * pivotX, h * (1.0f - pivotY), 0);
		Vector3 localRB = Vector3(w * (1.0f - pivotX), h * (1.0f - pivotY), 0);

		Vector3 lt = transform.worldMatrix.MultiplyPoint3x4(localLT);
		Vector3 rt = transform.worldMatrix.MultiplyPoint3x4(localRT);
		Vector3 lb = transform.worldMatrix.MultiplyPoint3x4(localLB);
		Vector3 rb = transform.worldMatrix.MultiplyPoint3x4(localRB);

		DrawModiGraphF(
			lt.x, lt.y,
			rt.x, rt.y,
			rb.x, rb.y,
			lb.x, lb.y,
			sprite.handle,
			TRUE
		);
	}
}