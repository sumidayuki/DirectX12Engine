#include "CollisionSystem.h"
#include "SpatialHashGrid.h"

void CollisionSystem::CheckCollision(CircleCollider2D& a, Transform& transformA, CircleCollider2D& b, Transform& transformB, Entity* entityB)
{
	float dx = transformA.position.x - transformB.position.x;
	float dy = transformA.position.y - transformB.position.y;

	float distanceSquared = dx * dx + dy * dy;

	float radiusSum = a.radius + b.radius;

	if (distanceSquared <= (radiusSum * radiusSum)) 
	{
		// circleAのステートを確認します。
		switch (a.info.state)
		{
			// ステートが None なら
		case CollisionState::None:
			// ステートを Enter に変更します。 
			a.info.state = CollisionState::Enter;
			// other に entityB を代入します。
			a.info.other = entityB;
			break;

			// ステートが Enter なら
		case CollisionState::Enter:
			// ステートを Stay に変更します。
			a.info.state = CollisionState::Stay;
			break;

		default:
			break;
		}
	}
	// 当たっていない時
	else
	{
		if (a.info.other == entityB || a.info.other == nullptr)
		{
			// circleAのステートを確認します。
			switch (a.info.state)
			{
				// ステートは Enter 又は Stay なら
			case CollisionState::Enter:
			case CollisionState::Stay:
				// ステートを Exit に変更します。
				a.info.state = CollisionState::Exit;
				break;

				// ステートがExitなら
			case CollisionState::Exit:
				// ステートを None に変更します。
				a.info.state = CollisionState::None;
				break;

			default:
				// ステートを None に変更します。
				a.info.state = CollisionState::None;
				break;
			}
		}
	}
}

void CollisionSystem::CheckCollision(CircleCollider2D& a, Transform& transformA, BoxCollider2D& b, Transform& transformB, Entity* entityB)
{
	float halfWidth = b.rect.x / 2.0f;
	float halfHeight = b.rect.y / 2.0f;
	float closestX = std::clamp(transformA.position.x, transformB.position.x - halfWidth, transformB.position.x + halfWidth);
	float closestY = std::clamp(transformA.position.y, transformB.position.y - halfHeight, transformB.position.y + halfHeight);

	float dx = transformA.position.x - closestX;
	float dy = transformA.position.y - closestY;

	if ((dx * dx + dy * dy) <= (a.radius * a.radius)) 
	{
		// a のステートを確認します。
		switch (a.info.state)
		{
			// ステートが None なら
		case CollisionState::None:
			// ステートを Enter に変更します。 
			a.info.state = CollisionState::Enter;
			// other に entityB を代入します。
			a.info.other = entityB;
			break;

			// ステートが Enter なら
		case CollisionState::Enter:
			// ステートを Stay に変更します。
			a.info.state = CollisionState::Stay;
			break;

		default:
			break;
		}
	}
	// 当たっていない時
	else
	{
		if (a.info.other->id == entityB->id && a.info.other->generation == entityB->generation || a.info.other == nullptr)
		{
			// a のステートを確認します。
			switch (a.info.state)
			{
				// ステートは Enter 又は Stay なら
			case CollisionState::Enter:
			case CollisionState::Stay:
				// ステートを Exit に変更します。
				a.info.state = CollisionState::Exit;
				break;

				// ステートがExitなら
			case CollisionState::Exit:
				// ステートを None に変更します。
				a.info.state = CollisionState::None;
				break;

			default:
				// ステートを None に変更します。
				a.info.state = CollisionState::None;
				break;
			}
		}
	}
}

void CollisionSystem::CheckCollision(BoxCollider2D& a, Transform& transformA, BoxCollider2D& b, Transform& transformB, Entity* entityB)
{
	float dx = std::abs(transformA.position.x - transformB.position.x);
	float dy = std::abs(transformA.position.y - transformB.position.y);
	float totalWidth = (a.rect.x + b.rect.x) / 2.0f;
	float totalHeight = (a.rect.y + b.rect.y) / 2.0f;

	if (dx <= totalWidth && dy <= totalHeight) 
	{
		// a のステートを確認します。
		switch (a.info.state)
		{
			// ステートが None なら
		case CollisionState::None:
			// ステートを Enter に変更します。 
			a.info.state = CollisionState::Enter;
			// other に entityB を代入します。
			a.info.other = entityB;
			break;

			// ステートが Enter なら
		case CollisionState::Enter:
			// ステートを Stay に変更します。
			a.info.state = CollisionState::Stay;
			break;

		default:
			break;
		}
	}
	// 当たっていない時
	else
	{
		if (a.info.other->id == entityB->id && a.info.other->generation == entityB->generation || a.info.other == nullptr)
		{
			// a のステートを確認します。
			switch (a.info.state)
			{
				// ステートは Enter 又は Stay なら
			case CollisionState::Enter:
			case CollisionState::Stay:
				// ステートを Exit に変更します。
				a.info.state = CollisionState::Exit;
				break;

				// ステートがExitなら
			case CollisionState::Exit:
				// ステートを None に変更します。
				a.info.state = CollisionState::None;
				break;

			default:
				// ステートを None に変更します。
				a.info.state = CollisionState::None;
				break;
			}
		}
	}
}

void CollisionSystem::CheckCollision(BoxCollider2D& a, Transform& transformA, CircleCollider2D& b, Transform& transformB, Entity* entityB)
{
	float halfWidth = a.rect.x / 2.0f;
	float halfHeight = a.rect.y / 2.0f;
	float closestX = std::clamp(transformB.position.x, transformA.position.x - halfWidth, transformA.position.x + halfWidth);
	float closestY = std::clamp(transformB.position.y, transformA.position.y - halfHeight, transformA.position.y + halfHeight);

	float dx = transformB.position.x - closestX;
	float dy = transformB.position.y - closestY;

	if ((dx * dx + dy * dy) <= (b.radius * b.radius)) 
	{
		// a のステートを確認します。
		switch (a.info.state)
		{
			// ステートが None なら
		case CollisionState::None:
			// ステートを Enter に変更します。 
			a.info.state = CollisionState::Enter;
			// other に entityB を代入します。
			a.info.other = entityB;
			break;

			// ステートが Enter なら
		case CollisionState::Enter:
			// ステートを Stay に変更します。
			a.info.state = CollisionState::Stay;
			break;

		default:
			break;
		}
	}
	// 当たっていない時
	else
	{
		if (a.info.other == entityB || a.info.other == nullptr)
		{
			// a のステートを確認します。
			switch (a.info.state)
			{
				// ステートは Enter 又は Stay なら
			case CollisionState::Enter:
			case CollisionState::Stay:
				// ステートを Exit に変更します。
				a.info.state = CollisionState::Exit;
				break;

				// ステートがExitなら
			case CollisionState::Exit:
				// ステートを None に変更します。
				a.info.state = CollisionState::None;
				break;

			default:
				// ステートを None に変更します。
				a.info.state = CollisionState::None;
				break;
			}
		}
	}
}

void CollisionSystem::Update(ComponentManager& cm, World& world)
{
	View<CircleCollider2D, Transform> circleView(cm);
	View<BoxCollider2D, Transform> boxView(cm);

	SpatialHashGrid grid(64.0f);

	for (auto [entity, collider, transform] : circleView) 
	{
		if (!entity.enabled) continue;

		if (!world.IsAlive(collider.info.other))
		{
			collider.info.other = nullptr;
		}

		grid.AddEntity(entity, Vector2{ transform.position.x, transform.position.y });
	}

	for (auto [entity, collider, transform] : boxView) 
	{
		if (!entity.enabled) continue;
		grid.AddEntity(entity, Vector2{ transform.position.x, transform.position.y });
	}

	for (auto [entityA, circleA, transformA] : circleView)
	{
		if (!entityA.enabled) continue;
		auto nearEntities = grid.GetNearbyEntities(Vector2{ transformA.position.x, transformA.position.y });
		for (Entity entityB : nearEntities) 
		{
			if (entityA == entityB || !world.IsAlive(&entityB)) continue;

			if (cm.HasComponents<CircleCollider2D, Transform>(entityB)) 
			{
				auto* circleB = cm.GetComponent<CircleCollider2D>(entityB);
				auto* transformB = cm.GetComponent<Transform>(entityB);
				CheckCollision(circleA, transformA, *circleB, *transformB, &entityB);
			}
			else if (cm.HasComponents<BoxCollider2D, Transform>(entityB)) 
			{
				auto* boxB = cm.GetComponent<BoxCollider2D>(entityB);
				auto* transformB = cm.GetComponent<Transform>(entityB);
				CheckCollision(circleA, transformA, *boxB, *transformB, &entityB);
			}
		}
	}

	for (auto [entityA, boxA, transformA] : boxView) 
	{
		if (!entityA.enabled) continue;
		auto nearEntities = grid.GetNearbyEntities(Vector2{ transformA.position.x, transformA.position.y });
		for (Entity entityB : nearEntities) 
		{
			if (entityA == entityB || !world.IsAlive(&entityB)) continue;

			if (cm.HasComponents<BoxCollider2D, Transform>(entityB)) 
			{
				auto* boxB = cm.GetComponent<BoxCollider2D>(entityB);
				auto* transformB = cm.GetComponent<Transform>(entityB);
				CheckCollision(boxA, transformA, *boxB, *transformB, &entityB);
			}
			else if (cm.HasComponents<CircleCollider2D, Transform>(entityB)) 
			{
				auto* circleB = cm.GetComponent<CircleCollider2D>(entityB);
				auto* transformB = cm.GetComponent<Transform>(entityB);
				CheckCollision(boxA, transformA, *circleB, *transformB, &entityB);
			}
		}
	}
}