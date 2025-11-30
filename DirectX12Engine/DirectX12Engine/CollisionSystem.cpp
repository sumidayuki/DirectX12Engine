#include "CollisionSystem.h"
#include "SpatialHashGrid.h"
#include "SpatialHashGrid2D.h"
#include "DebugManager.h"

void CollisionSystem::UpdateComponentState(CollisionInfo& info, Entity targetEntity, bool isHitNow)
{
	if (isHitNow)
	{
		// 新規衝突、または継続衝突
		if (info.state == CollisionState::None || info.state == CollisionState::Exit)
		{
			info.state = CollisionState::Enter;
			info.other = targetEntity;
		}
		else if (info.state == CollisionState::Enter || info.state == CollisionState::Stay)
		{
			info.state = CollisionState::Stay;
			// 必要であればここで相手が変わった場合の処理（今回は割愛）
		}
	}
	else
	{
		// 今当たっていないが、前回ターゲットとして記録されていた場合（Exit処理）
		if (info.other == targetEntity)
		{
			if (info.state == CollisionState::Enter || info.state == CollisionState::Stay)
			{
				info.state = CollisionState::Exit;
			}
			else if (info.state == CollisionState::Exit)
			{
				info.state = CollisionState::None;
				info.other = INVALID_ENTITY;
			}
		}
	}
}

bool CollisionSystem::CheckPair(Entity entityA, Entity entityB, World& world)
{
	auto* transA = world.GetComponent<Transform>(entityA);
	auto* transB = world.GetComponent<Transform>(entityB);
	if (!transA || !transB) return false;

	Vector3 posA = TransformSystem::GetInstance()->GetPosition(*transA);
	Vector3 posB = TransformSystem::GetInstance()->GetPosition(*transB);

	// Aが Sphere の場合
	if (world.HasComponent<SphereCollider>(entityA))
	{
		auto& colA = *world.GetComponent<SphereCollider>(entityA);
		Vector3 worldPosA = posA + colA.offset;

		if (world.HasComponent<SphereCollider>(entityB)) // Sphere vs Sphere
		{
			auto& colB = *world.GetComponent<SphereCollider>(entityB);
			return IsColliding(colA, worldPosA, colB, posB + colB.offset);
		}
		else if (world.HasComponent<AABBCollider>(entityB)) // Sphere vs AABB
		{
			auto& colB = *world.GetComponent<AABBCollider>(entityB);
			return IsColliding(colA, worldPosA, colB, posB + colB.offset);
		}
	}
	// Aが AABB の場合
	else if (world.HasComponent<AABBCollider>(entityA))
	{
		auto& colA = *world.GetComponent<AABBCollider>(entityA);
		Vector3 worldPosA = posA + colA.offset;

		if (world.HasComponent<SphereCollider>(entityB)) // AABB vs Sphere
		{
			auto& colB = *world.GetComponent<SphereCollider>(entityB);
			// 引数の順序を入れ替えて再利用
			return IsColliding(colB, posB + colB.offset, colA, worldPosA);
		}
		else if (world.HasComponent<AABBCollider>(entityB)) // AABB vs AABB
		{
			auto& colB = *world.GetComponent<AABBCollider>(entityB);
			return IsColliding(colA, worldPosA, colB, posB + colB.offset);
		}
	}

	return false;
}

bool CollisionSystem::IsColliding(const SphereCollider& a, const Vector3& posA, const SphereCollider& b, const Vector3& posB)
{
	return GeometryUtility::SphereVSSphere(posA, a.radius, posB, b.radius);
}

bool CollisionSystem::IsColliding(const AABBCollider& a, const Vector3& posA, const AABBCollider& b, const Vector3& posB)
{
	Vector3 halfA = a.bounds.GetSize() * 0.5f;
	Vector3 halfB = b.bounds.GetSize() * 0.5f;

	// Min/Max の計算
	Vector3 minA = posA - halfA;
	Vector3 maxA = posA + halfA;
	Vector3 minB = posB - halfB;
	Vector3 maxB = posB + halfB;

	return GeometryUtility::AABBVSAABB(minA, maxA, minB, maxB);
}

bool CollisionSystem::IsColliding(const SphereCollider& sphere, const Vector3& spherePos, const AABBCollider& aabb, const Vector3& aabbPos)
{
	// AABBとSphereの判定ロジック（簡易実装：AABB上の最近接点を探す）
	Vector3 halfBox = aabb.bounds.GetSize() * 0.5f;
	Vector3 minBox = aabbPos - halfBox;
	Vector3 maxBox = aabbPos + halfBox;

	// クランプ処理で最近接点を求める
	float closestX = std::max(minBox.x, std::min(spherePos.x, maxBox.x));
	float closestY = std::max(minBox.y, std::min(spherePos.y, maxBox.y));
	float closestZ = std::max(minBox.z, std::min(spherePos.z, maxBox.z));

	Vector3 closestPoint(closestX, closestY, closestZ);
	float distanceSq = (closestPoint - spherePos).SqrMagnitude();

	return distanceSq <= (sphere.radius * sphere.radius);
}

void CollisionSystem::Update(World& world)
{
	static SpatialHashGrid grid(1000.0f);
	grid.Clear();

	View<SphereCollider, Transform> sphereView(world);
	View<AABBCollider, Transform> aabbView(world);

	// Sphereの登録とデバッグ描画
	for (auto [entity, collider, transform] : sphereView)
	{
		if (!entity.enabled) continue;
		Vector3 center = TransformSystem::GetInstance()->GetPosition(transform) + collider.offset;
		grid.AddEntity(entity, center);

		// 色の決定ロジックも共通化可能ですが、ここでは簡易的に
		Color c = (collider.info.state == CollisionState::Enter || collider.info.state == CollisionState::Stay) ? Color::red : Color::green;
		DebugManager::GetInstance()->DrawSphere(center, collider.radius, c);
	}

	// AABBの登録とデバッグ描画
	for (auto [entity, collider, transform] : aabbView)
	{
		if (!entity.enabled) continue;
		Vector3 center = TransformSystem::GetInstance()->GetPosition(transform) + collider.offset;
		grid.AddEntity(entity, center);

		Vector3 half = collider.bounds.GetSize() * 0.5f;
		Color c = (collider.info.state == CollisionState::Enter || collider.info.state == CollisionState::Stay) ? Color::red : Color::green;
		DebugManager::GetInstance()->DrawAABB(center - half, center + half, c);
	}

	auto processCollisions = [&](Entity entityA, CollisionInfo& infoA, Transform& transformA)
		{
			auto nearEntities = grid.GetNearbyEntities(transformA.position);

			Entity previousOther = infoA.other;
			bool isHitThisFrame = false;
			Entity hitTarget = INVALID_ENTITY;

			for (Entity entityB : nearEntities)
			{
				if (entityA.id == entityB.id || !world.IsAlive(entityB)) continue;

				// CheckPairを使うことで、相手がSphereでもAABBでも分岐なしで判定可能
				if (CheckPair(entityA, entityB, world))
				{
					isHitThisFrame = true;
					hitTarget = entityB;

					// 必要なら相手側のStateもここで更新できますが、
					// 基本的には「自分から見て当たったか」を全員分回せば整合性は取れます
					break; // 単一衝突判定の場合はbreak。複数衝突対応ならリスト化が必要
				}
			}

			// 判定結果に基づいて状態更新を一括で行う
			Entity targetToUpdate = isHitThisFrame ? hitTarget : previousOther;
			UpdateComponentState(infoA, targetToUpdate, isHitThisFrame);
		};

	// Sphereについて回す
	for (auto [entity, collider, transform] : sphereView) {
		if (entity.enabled) processCollisions(entity, collider.info, transform);
	}

	// AABBについて回す
	for (auto [entity, collider, transform] : aabbView) {
		if (entity.enabled) processCollisions(entity, collider.info, transform);
	}
}