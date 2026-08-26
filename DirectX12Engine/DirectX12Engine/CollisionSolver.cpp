#include "Precompiled.h"
#include "CollisionSolver.h"
#include "CollisionMatrix.h"
#include "DebugManager.h"

Vector3 CollisionSolver::GetScaledHalfExtents(const Collider& col, const Transform& trans) const
{
	return Vector3(
		col.size.x * trans.scale.x * 0.5f,
		col.size.y * trans.scale.y * 0.5f,
		col.size.z * trans.scale.z * 0.5f
	);
}

float CollisionSolver::GetScaledRadius(const Collider& col, const Transform& trans) const
{
	float maxScale = std::max({ trans.scale.x, trans.scale.y, trans.scale.z });
	return col.radius * maxScale;
}

bool CollisionSolver::TestSphereSphere(const Vector3& posA, float radiusA, const Vector3& posB, float radiusB, Vector3& outNormal, float& outPenetration, Vector3& outPoint)
{
	Vector3 diff = posA - posB;
	float distSq = diff.SqrMagnitude();
	float radSum = radiusA + radiusB;

	if (distSq >= radSum * radSum)
		return false;

	float dist = std::sqrt(distSq);

	if (dist < 0.0001f)
	{
		outNormal = Vector3::up;
		outPenetration = radSum;
		outPoint = posA;
	}
	else
	{
		outNormal = diff / dist;                 // 法線はAからBへの方向 
		outPenetration = radSum - dist;
		outPoint = posB + outNormal * radiusB;   // 接触点はBの表面上の点（Aの中心からBの中心への線上）
	}

	return true;
}

bool CollisionSolver::TestAABBAABB(const Vector3& posA, const Vector3& halfExtA, const Vector3& posB, const Vector3& halfExtB, Vector3& outNormal, float& outPenetration, Vector3& outPoint)
{
	Vector3 diff = posA - posB;

	float overlapX = (halfExtA.x + halfExtB.x) - std::abs(diff.x);
	if (overlapX <= 0.0f) return false;

	float overlapY = (halfExtA.y + halfExtB.y) - std::abs(diff.y);
	if (overlapY <= 0.0f) return false;

	float overlapZ = (halfExtA.z + halfExtB.z) - std::abs(diff.z);
	if (overlapZ <= 0.0f) return false;

	// 最小のオーバーラップ軸を見つける
	outNormal = Vector3::zero;

	if (overlapX < overlapY && overlapX < overlapZ)
	{
		outPenetration = overlapX;
		outNormal.x = (diff.x > 0.0f) ? 1.0f : -1.0f;
	}
	else if (overlapY < overlapZ)
	{
		outPenetration = overlapY;
		outNormal.y = (diff.y > 0.0f) ? 1.0f : -1.0f;
	}
	else
	{
		outPenetration = overlapZ;
		outNormal.z = (diff.z > 0.0f) ? 1.0f : -1.0f;
	}

	// 接触点は、両方のAABBの重なり部分の中心に設定します
	Vector3 minA = posA - halfExtA;
	Vector3 maxA = posA + halfExtA;
	Vector3 minB = posB - halfExtB;
	Vector3 maxB = posB + halfExtB;

	outPoint = Vector3(
		(std::max(minA.x, minB.x) + std::min(maxA.x, maxB.x)) * 0.5f,
		(std::max(minA.y, minB.y) + std::min(maxA.y, maxB.y)) * 0.5f,
		(std::max(minA.z, minB.z) + std::min(maxA.z, maxB.z)) * 0.5f
	);

	return true;
}

bool CollisionSolver::TestOBBOBB(const Vector3& posA, const Vector3& halfExtA, const Matrix4x4& rotA, const Vector3& posB, const Vector3& halfExtB, const Matrix4x4& rotB, Vector3& outNormal, float& outPenetration, Vector3& outPoint)
{
	// OBBのローカル軸を抽出
		// rotAとrotBは回転行列であると仮定します。各列がローカル軸を表すとします。
	Vector3 axesA[3] = {
		Vector3(rotA.m[0][0], rotA.m[1][0], rotA.m[2][0]).Normalized(),
		Vector3(rotA.m[0][1], rotA.m[1][1], rotA.m[2][1]).Normalized(),
		Vector3(rotA.m[0][2], rotA.m[1][2], rotA.m[2][2]).Normalized()
	};

	Vector3 axesB[3] = {
		Vector3(rotB.m[0][0], rotB.m[1][0], rotB.m[2][0]).Normalized(),
		Vector3(rotB.m[0][1], rotB.m[1][1], rotB.m[2][1]).Normalized(),
		Vector3(rotB.m[0][2], rotB.m[1][2], rotB.m[2][2]).Normalized()
	};

	float extA[3] = { halfExtA.x, halfExtA.y, halfExtA.z };
	float extB[3] = { halfExtB.x, halfExtB.y, halfExtB.z };

	Vector3 T = posB - posA;

	// OBBの軸間の回転行列とその絶対値を計算
	float R[3][3], AbsR[3][3];
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			R[i][j] = Vector3::Dot(axesA[i], axesB[j]);
			AbsR[i][j] = std::abs(R[i][j]) + 1e-6f; // 数値的な安定性のための小さな値
		}
	}

	float minOverlap = Mathf::Infinity;
	Vector3 minAxis = Vector3::zero;

	auto TestAxis = [&](const Vector3& axis, float ra, float rb, const Vector3& t) -> bool
		{
			float axisMag = axis.Magnitude();
			if (axisMag < 0.001f) return true;

			Vector3 normAxis = axis / axisMag;
			float projT = std::abs(Vector3::Dot(t, normAxis));
			float overlap = (ra + rb) - projT;

			if (overlap <= 0.0f)
				return false;

			if (overlap < minOverlap)
			{
				minOverlap = overlap;
				minAxis = normAxis;

				// normal は B -> A の方向に統一する
				if (Vector3::Dot(T, normAxis) < 0.0f)
				{
					minAxis = -minAxis;
				}
			}
			return true;
		};

	// 15軸をテスト：3軸A、3軸B、9軸のクロス積
	for (int i = 0; i < 3; ++i)
	{
		float ra = extA[i];
		float rb = extB[0] * AbsR[i][0] + extB[1] * AbsR[i][1] + extB[2] * AbsR[i][2];
		if (!TestAxis(axesA[i], ra, rb, T)) return false;
	}

	// Bの軸
	for (int j = 0; j < 3; ++j)
	{
		float ra = extA[0] * AbsR[0][j] + extA[1] * AbsR[1][j] + extA[2] * AbsR[2][j];
		float rb = extB[j];
		if (!TestAxis(axesB[j], ra, rb, T)) return false;
	}

	// クロス積軸
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			Vector3 crossAxis = Vector3::Cross(axesA[i], axesB[j]);

			float ra = 0.0f, rb = 0.0f;
			for (int k = 0; k < 3; ++k)
			{
				if (k != i)
					ra += extA[k] * std::abs(Vector3::Dot(axesA[k], crossAxis));
				if (k != j)
					rb += extB[k] * std::abs(Vector3::Dot(axesB[k], crossAxis));
			}

			if (!TestAxis(crossAxis, ra, rb, T)) return false;
		}
	}

	// 最小のオーバーラップ軸が衝突の法線になります
	outNormal = minAxis;
	outPenetration = minOverlap;

	// 接触点は、両方のOBBの重なり部分の中心に設定します
	outPoint = posA + minAxis * (-outPenetration * 0.5f);

	return true;
}

bool CollisionSolver::TestSphereAABB(const Vector3& spherePos, float sphereRadius, const Vector3& boxPos, const Vector3& boxHalfExt, Vector3& outNormal, float& outPenetration, Vector3& outPoint)
{
	Vector3 boxMin = boxPos - boxHalfExt;
	Vector3 boxMax = boxPos + boxHalfExt;

	// 球の中心からAABBへの最近接点を見つける
	Vector3 closest;
	closest.x = std::max(boxMin.x, std::min(spherePos.x, boxMax.x));
	closest.y = std::max(boxMin.y, std::min(spherePos.y, boxMax.y));
	closest.z = std::max(boxMin.z, std::min(spherePos.z, boxMax.z));

	Vector3 diff = spherePos - closest;
	float distSq = diff.SqrMagnitude();

	if (distSq >= sphereRadius * sphereRadius)
		return false;

	float dist = std::sqrt(distSq);

	if (dist < 0.0001f)
	{
		// 球の中心がAABBの内部にある場合、最も浅い軸を見つけて法線と貫通深度を設定します
		float dx = std::min(spherePos.x - boxMin.x, boxMax.x - spherePos.x);
		float dy = std::min(spherePos.y - boxMin.y, boxMax.y - spherePos.y);
		float dz = std::min(spherePos.z - boxMin.z, boxMax.z - spherePos.z);

		outNormal = Vector3::zero;
		if (dx < dy && dx < dz)
		{
			outNormal.x = (spherePos.x - boxPos.x > 0.0f) ? 1.0f : -1.0f;
			outPenetration = dx + sphereRadius;
		}
		else if (dy < dz)
		{
			outNormal.y = (spherePos.y - boxPos.y > 0.0f) ? 1.0f : -1.0f;
			outPenetration = dy + sphereRadius;
		}
		else
		{
			outNormal.z = (spherePos.z - boxPos.z > 0.0f) ? 1.0f : -1.0f;
			outPenetration = dz + sphereRadius;
		}
		outPoint = closest;
	}
	else
	{
		outNormal = diff / dist;
		outPenetration = sphereRadius - dist;
		outPoint = closest;
	}

	return true;
}

bool CollisionSolver::TestSphereOBB(const Vector3& spherePos, float sphereRadius, const Vector3& boxPos, const Vector3& boxHalfExt, const Matrix4x4& boxRot, Vector3& outNormal, float& outPenetration, Vector3& outPoint)
{
	Vector3 axesB[3] = {
			Vector3(boxRot.m[0][0], boxRot.m[1][0], boxRot.m[2][0]).Normalized(),
			Vector3(boxRot.m[0][1], boxRot.m[1][1], boxRot.m[2][1]).Normalized(),
			Vector3(boxRot.m[0][2], boxRot.m[1][2], boxRot.m[2][2]).Normalized()
	};

	Vector3 diff = spherePos - boxPos;
	Vector3 localSphere(
		Vector3::Dot(diff, axesB[0]),
		Vector3::Dot(diff, axesB[1]),
		Vector3::Dot(diff, axesB[2])
	);

	Vector3 localClosest(
		std::max(-boxHalfExt.x, std::min(localSphere.x, boxHalfExt.x)),
		std::max(-boxHalfExt.y, std::min(localSphere.y, boxHalfExt.y)),
		std::max(-boxHalfExt.z, std::min(localSphere.z, boxHalfExt.z))
	);

	Vector3 worldClosest = boxPos
		+ axesB[0] * localClosest.x
		+ axesB[1] * localClosest.y
		+ axesB[2] * localClosest.z;

	Vector3 toSphere = spherePos - worldClosest;
	float distSq = toSphere.SqrMagnitude();

	if (distSq >= sphereRadius * sphereRadius)
		return false;

	float dist = std::sqrt(distSq);

	if (dist < 0.0001f)
	{
		float minPen = Mathf::Infinity;
		int minIdx = 0;
		float localDists[6] = {
			boxHalfExt.x - localSphere.x, boxHalfExt.x + localSphere.x,
			boxHalfExt.y - localSphere.y, boxHalfExt.y + localSphere.y,
			boxHalfExt.z - localSphere.z, boxHalfExt.z + localSphere.z
		};
		for (int i = 0; i < 6; ++i)
		{
			if (localDists[i] < minPen)
			{
				minPen = localDists[i];
				minIdx = i;
			}
		}

		int axisIdx = minIdx / 2;
		float sign = (minIdx % 2 == 0) ? 1.0f : -1.0f;
		outNormal = axesB[axisIdx] * sign;
		outPenetration = minPen + sphereRadius;
		outPoint = worldClosest;
	}
	else
	{
		outNormal = toSphere / dist;
		outPenetration = sphereRadius - dist;
		outPoint = worldClosest;
	}

	return true;
}

bool CollisionSolver::TestAABBOBB(const Vector3& aabbPos, const Vector3& aabbHalfExt, const Vector3& obbPos, const Vector3& obbHalfExt, const Matrix4x4& obbRot, Vector3& outNormal, float& outPenetration, Vector3& outPoint)
{
	// AABBをOBBのローカル空間に変換して、AABB vs OBBテストを行います。
	return TestOBBOBB(
		aabbPos, aabbHalfExt, Matrix4x4::identity,
		obbPos, obbHalfExt, obbRot,
		outNormal, outPenetration, outPoint);
}

bool CollisionSolver::TestPair(const Collider& colA, const Transform& transA, const Collider& colB, const Transform& transB, ContactInfo& outContactA, ContactInfo& outContactB)
{
	Vector3 posA = transA.position + colA.offset;
	Vector3 posB = transB.position + colB.offset;
	Vector3 normal, point;
	float penetration;
	bool hit = false;

	// コライダの種類に応じて適切なテスト関数を呼び出します
	if (colA.type == ColliderType::Sphere && colB.type == ColliderType::Sphere)
	{
		float rA = GetScaledRadius(colA, transA);
		float rB = GetScaledRadius(colB, transB);
		hit = TestSphereSphere(posA, rA, posB, rB, normal, penetration, point);
	}
	else if (colA.type == ColliderType::AABB && colB.type == ColliderType::AABB)
	{
		Vector3 hA = GetScaledHalfExtents(colA, transA);
		Vector3 hB = GetScaledHalfExtents(colB, transB);
		hit = TestAABBAABB(posA, hA, posB, hB, normal, penetration, point);
	}
	else if (colA.type == ColliderType::Box && colB.type == ColliderType::Box)
	{
		// OBB vs OBB
		Vector3 hA = GetScaledHalfExtents(colA, transA);
		Vector3 hB = GetScaledHalfExtents(colB, transB);
		Matrix4x4 rotA = Matrix4x4::Rotate(transA.rotation);
		Matrix4x4 rotB = Matrix4x4::Rotate(transB.rotation);
		hit = TestOBBOBB(posA, hA, rotA, posB, hB, rotB, normal, penetration, point);
	}
	else if (colA.type == ColliderType::Sphere && colB.type == ColliderType::AABB)
	{
		float rA = GetScaledRadius(colA, transA);
		Vector3 hB = GetScaledHalfExtents(colB, transB);
		hit = TestSphereAABB(posA, rA, posB, hB, normal, penetration, point);
	}
	else if (colA.type == ColliderType::AABB && colB.type == ColliderType::Sphere)
	{
		float rB = GetScaledRadius(colB, transB);
		Vector3 hA = GetScaledHalfExtents(colA, transA);
		hit = TestSphereAABB(posB, rB, posA, hA, normal, penetration, point);
		normal = -normal;
	}
	else if (colA.type == ColliderType::Sphere && colB.type == ColliderType::Box)
	{
		// Sphere vs OBB
		float rA = GetScaledRadius(colA, transA);
		Vector3 hB = GetScaledHalfExtents(colB, transB);
		Matrix4x4 rotB = Matrix4x4::Rotate(transB.rotation);
		hit = TestSphereOBB(posA, rA, posB, hB, rotB, normal, penetration, point);
	}
	else if (colA.type == ColliderType::Box && colB.type == ColliderType::Sphere)
	{
		float rB = GetScaledRadius(colB, transB);
		Vector3 hA = GetScaledHalfExtents(colA, transA);
		Matrix4x4 rotA = Matrix4x4::Rotate(transA.rotation);
		hit = TestSphereOBB(posB, rB, posA, hA, rotA, normal, penetration, point);
		normal = -normal;
	}
	else if (colA.type == ColliderType::AABB && colB.type == ColliderType::Box)
	{
		// AABB vs OBB
		Vector3 hA = GetScaledHalfExtents(colA, transA);
		Vector3 hB = GetScaledHalfExtents(colB, transB);
		Matrix4x4 rotB = Matrix4x4::Rotate(transB.rotation);
		hit = TestAABBOBB(posA, hA, posB, hB, rotB, normal, penetration, point);
	}
	else if (colA.type == ColliderType::Box && colB.type == ColliderType::AABB)
	{
		Vector3 hA = GetScaledHalfExtents(colA, transA);
		Vector3 hB = GetScaledHalfExtents(colB, transB);
		Matrix4x4 rotA = Matrix4x4::Rotate(transA.rotation);
		hit = TestAABBOBB(posB, hB, posA, hA, rotA, normal, penetration, point);
		normal = -normal;
	}

	if (hit)
	{
		// contactA.normal は B から A を押し出す方向
		outContactA.point = point;
		outContactA.normal = normal;
		outContactA.penetration = penetration;

		// contactB.normal は A から B を押し出す方向
		outContactB.point = point;
		outContactB.normal = -normal;
		outContactB.penetration = penetration;

		return true;
	}

	return false;
}

void CollisionSolver::Detect(World& world)
{
	m_stepPairs.clear();

	BuildBroadPhase(world);

	for (const CollisionPairKey& key : m_candidatePairs)
	{
		Entity entityA = key.entityA;
		Entity entityB = key.entityB;

		Collider* colA = world.GetComponent<Collider>(entityA);
		Collider* colB = world.GetComponent<Collider>(entityB);

		Transform* transA = world.GetComponent<Transform>(entityA);
		Transform* transB = world.GetComponent<Transform>(entityB);

		if (!colA || !colB || !transA || !transB)
		{
			continue;
		}

		Layer* layerCompA = world.GetComponent<Layer>(entityA);
		Layer* layerCompB = world.GetComponent<Layer>(entityB);
		LayerMask layerA = layerCompA ? layerCompA->layer : Layers::Default;
		LayerMask layerB = layerCompB ? layerCompB->layer : Layers::Default;

		if (!CollisionMatrix::GetInstance().CanCollide(layerA, layerB))
		{
			continue;
		}

		if ((colA->collisionMask & layerB) == 0 || (colB->collisionMask & layerA) == 0)
		{
			continue;
		}

		ContactInfo contactA;
		ContactInfo contactB;

		contactA.other = entityB;
		contactB.other = entityA;

		if (!TestPair(*colA, *transA, *colB, *transB, contactA, contactB))
		{
			continue;
		}

		CollisionPairData pairData;
		pairData.contactA = contactA;
		pairData.contactB = contactB;

		m_stepPairs[key] = pairData;
		m_framePairs[key] = pairData;
	}
}

void CollisionSolver::Solve(World& world)
{
	for (const auto& [key, pair] : m_stepPairs)
	{
		SolvePair(world, key.entityA, key.entityB, pair);
	}
}

void CollisionSolver::SolvePair(World& world, Entity entityA, Entity entityB, const CollisionPairData& pair)
{
	Collider* colA = world.GetComponent<Collider>(entityA);
	Collider* colB = world.GetComponent<Collider>(entityB);

	if (!colA || !colB)
	{
		return;
	}

	if (colA->isTrigger || colB->isTrigger)
	{
		return;
	}

	Transform* transA = world.GetComponent<Transform>(entityA);
	Transform* transB = world.GetComponent<Transform>(entityB);

	if (!transA || !transB)
	{
		return;
	}

	Rigidbody* rbA = world.GetComponent<Rigidbody>(entityA);
	Rigidbody* rbB = world.GetComponent<Rigidbody>(entityB);

	const bool movableA = rbA != nullptr && !rbA->isKinematic;

	const bool movableB = rbB != nullptr && !rbB->isKinematic;

	if (!movableA && !movableB)
	{
		return;
	}

	const Vector3 normalA = pair.contactA.normal;
	const Vector3 normalB = pair.contactB.normal;

	const float penetration = pair.contactA.penetration;

	// 位置補正
	if (movableA && movableB)
	{
		const float halfPenetration = penetration * 0.5f;

		transA->position += normalA * halfPenetration;
		transB->position += normalB * halfPenetration;

		transA->dirty = true;
		transB->dirty = true;
	}
	else if (movableA)
	{
		transA->position += normalA * penetration;
		transA->dirty = true;
	}
	else if (movableB)
	{
		transB->position += normalB * penetration;
		transB->dirty = true;
	}


	// 速度補正
	if (movableA)
	{
		const float normalVelocity = Vector3::Dot(rbA->velocity, normalA);

		if (normalVelocity < 0.0f)
		{
			rbA->velocity -= normalA * normalVelocity;
		}
	}

	if (movableB)
	{
		const float normalVelocity = Vector3::Dot(rbB->velocity, normalB);

		if (normalVelocity < 0.0f)
		{
			rbB->velocity -= normalB * normalVelocity;
		}
	}
}

void CollisionSolver::AddContact(CollisionEvents* events, const ContactInfo& info, CollisionState state)
{
	if (!events)
	{
		return;
	}

	switch (state)
	{
	case CollisionState::Enter:
		if (events->enterCount < CollisionEvents::MAX_CONTACTS)
		{
			events->enterContacts[events->enterCount++] = info;
		}

		break;

	case CollisionState::Stay:
		if (events->stayCount < CollisionEvents::MAX_CONTACTS)
		{
			events->stayContacts[events->stayCount++] = info;
		}

		break;

	case CollisionState::Exit:
		if (events->exitCount < CollisionEvents::MAX_CONTACTS)
		{
			events->exitContacts[events->exitCount++] = info;
		}

		break;

	default:
		break;
	}
}

BroadPhaseBounds CollisionSolver::CalculateBounds(const Collider& col, const Transform& trans) const
{
	Vector3 center = trans.position + col.offset;

	// 球
	if (col.type == ColliderType::Sphere)
	{
		float r = GetScaledRadius(col, trans);
		Vector3 extent(r, r, r);

		return { center - extent, center + extent };
	}

	Vector3 halfExtents = GetScaledHalfExtents(col, trans);

	// AABB
	if (col.type == ColliderType::AABB)
	{
		return { center - halfExtents, center + halfExtents };
	}

	// OBB
	Matrix4x4 rotaion = Matrix4x4::Rotate(trans.rotation);

	Vector3 axisX(rotaion.m[0][0], rotaion.m[1][0], rotaion.m[2][0]);
	Vector3 axisY(rotaion.m[0][1], rotaion.m[1][1], rotaion.m[2][1]);
	Vector3 axisZ(rotaion.m[0][2], rotaion.m[1][2], rotaion.m[2][2]);

	Vector3 worldExtents;

	worldExtents.x = std::abs(axisX.x) * halfExtents.x + std::abs(axisY.x) * halfExtents.y + std::abs(axisZ.x) * halfExtents.z;
	worldExtents.y = std::abs(axisX.y) * halfExtents.x + std::abs(axisY.y) * halfExtents.y + std::abs(axisZ.y) * halfExtents.z;
	worldExtents.z = std::abs(axisX.z) * halfExtents.x + std::abs(axisY.z) * halfExtents.y + std::abs(axisZ.z) * halfExtents.z;

	return { center - worldExtents, center + worldExtents };
}

void CollisionSolver::BuildBroadPhase(World& world)
{
	m_broadPhase.Clear();

	View<Collider, Transform> view(world);

	for (auto [entity, collider, transform] : view)
	{
		if (!collider.isEnable)
		{
			continue;
		}

		BroadPhaseBounds bounds = CalculateBounds(collider, transform);

		m_broadPhase.Insert(entity, bounds);
	}

	m_broadPhase.GeneratePairs(m_candidatePairs);
}

void CollisionSolver::BeginFrame(World& world)
{
	m_framePairs.clear();
	m_stepPairs.clear();

	View<CollisionEvents> view(world);

	for (auto [entity, events] : view)
	{
		events.enterCount = 0;
		events.stayCount = 0;
		events.exitCount = 0;
	}

	View<Collider> colliderView(world);

	for (auto [entity, collider] : colliderView)
	{
		collider.info.state = CollisionState::None;
		collider.info.other = INVALID_ENTITY;
	}
}

void CollisionSolver::Step(World& world)
{
	Detect(world);
	Solve(world);
}

void CollisionSolver::EndFrame(World& world)
{
	for (const auto& [key, pair] : m_framePairs)
	{
		const bool existedLastFrame = m_previousPairs.contains(key);

		const CollisionState state = existedLastFrame ? CollisionState::Stay : CollisionState::Enter;

		Collider* colA = world.GetComponent<Collider>(key.entityA);
		if (colA)
		{
			colA->info.state = state;
			colA->info.other = key.entityB;
		}
		CollisionEvents* eventsA = world.GetComponent<CollisionEvents>(key.entityA);
		AddContact(eventsA, pair.contactA, state);

		Collider* colB = world.GetComponent<Collider>(key.entityB);
		if (colB)
		{
			colB->info.state = state;
			colB->info.other = key.entityA;
		}
		CollisionEvents* eventsB = world.GetComponent<CollisionEvents>(key.entityB);
		AddContact(eventsB, pair.contactB, state);
	}

	for (const auto& [key, oldPair] : m_previousPairs)
	{
		if (m_framePairs.contains(key))
			continue;

		ContactInfo exitA;
		exitA.other = key.entityB;

		ContactInfo exitB;
		exitB.other = key.entityA;

		AddContact(world.GetComponent<CollisionEvents>(key.entityA), exitA, CollisionState::Exit);

		AddContact(world.GetComponent<CollisionEvents>(key.entityB), exitB, CollisionState::Exit);
		
		if (Collider* colA = world.GetComponent<Collider>(key.entityA))
		{
			colA->info.state = CollisionState::Exit;
			colA->info.other = key.entityB;
		}

		if (Collider* colB = world.GetComponent<Collider>(key.entityB))
		{
			colB->info.state = CollisionState::Exit;
			colB->info.other = key.entityA;
		}
	}

	m_previousPairs = m_framePairs;

	DrawDebugColliders(world);
}

void CollisionSolver::DrawDebugColliders(World& world)
{
	View<Collider, Transform> view(world);

	for (auto [entity, col, transform] : view)
	{
		if (!col.isEnable) continue;

		Vector3 center = transform.position + col.offset;

		Color color = Color::green;
		if (col.info.state == CollisionState::Enter || col.info.state == CollisionState::Stay)
		{
			color = Color::red;
		}

		if (col.type == ColliderType::Sphere)
		{
			float r = GetScaledRadius(col, transform);
			DebugManager::GetInstance()->DrawSphere(center, r, color);
		}
		else if (col.type == ColliderType::AABB)
		{
			Vector3 halfExt = GetScaledHalfExtents(col, transform);
			DebugManager::GetInstance()->DrawAABB(center - halfExt, center + halfExt, color);
		}
		else if (col.type == ColliderType::Box)
		{
			Vector3 halfExt = GetScaledHalfExtents(col, transform);
			Matrix4x4 rotMat = Matrix4x4::Rotate(transform.rotation);

			Vector3 ax = Vector3(rotMat.m[0][0], rotMat.m[1][0], rotMat.m[2][0]).Normalized();
			Vector3 ay = Vector3(rotMat.m[0][1], rotMat.m[1][1], rotMat.m[2][1]).Normalized();
			Vector3 az = Vector3(rotMat.m[0][2], rotMat.m[1][2], rotMat.m[2][2]).Normalized();

			Vector3 ex = ax * halfExt.x;
			Vector3 ey = ay * halfExt.y;
			Vector3 ez = az * halfExt.z;

			Vector3 corners[8] = {
				center - ex - ey - ez,
				center + ex - ey - ez,
				center + ex + ey - ez,
				center - ex + ey - ez,
				center - ex - ey + ez,
				center + ex - ey + ez,
				center + ex + ey + ez,
				center - ex + ey + ez
			};

			int edges[12][2] = {
				{0,1}, {1,2}, {2,3}, {3,0},
				{4,5}, {5,6}, {6,7}, {7,4},
				{0,4}, {1,5}, {2,6}, {3,7}
			};

			for (int i = 0; i < 12; ++i)
			{
				DebugManager::GetInstance()->DrawLine(corners[edges[i][0]], corners[edges[i][1]], color);
			}
		}
	}
}
