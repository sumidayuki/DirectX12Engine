#include "TransformSystem.h"

void TransformSystem::Translate(Transform& transform, const Vector3& translation)
{
	transform.position += translation;
	transform.dirty = true;
}

void TransformSystem::Rotate(Transform& transform, const Vector3 axis, float angle)
{
	const Quaternion q = Quaternion::AngleAxis(angle, axis);

	transform.rotation = q * transform.rotation;
	transform.dirty = true;
}

void TransformSystem::Start(ComponentManager& cm, World& world)
{
	m_view = std::make_unique<View<Transform>>(cm);
}

void TransformSystem::Update(ComponentManager& cm, World& world)
{
	m_view->Update();

	for (auto&& [entity, transform] : *m_view)
	{
		// entity が enabled ではないならスキップします。（処理を行う必要がないため）
		if (!entity.enabled) continue;

		// dirty が false ならスキップします。（更新を掛ける必要がないため）
		if (!transform.dirty) continue;

		// スケール・回転・位置からローカル変換行列を計算します。
		transform.localMatrix.SetSRT(transform.scale, transform.rotation, transform.position);

		// 親エンティティが設定されているなら
		if (transform.parent != NullEntity)
		{
			// 親の Transform を取得します。
			const auto& parentTransform = world.GetComponent<Transform>(transform.parent);

			// 親のワールド行列を掛けて自身のワールド行列を算出します。
			transform.worldMatrix = transform.localMatrix * parentTransform->worldMatrix;
		}
		// されていないなら
		else
		{
			// ローカル行列をそのままワールド行列に代入します。
			transform.worldMatrix = transform.localMatrix;
		}

		// 更新が終わったので dirty フラグを false にする。（次のフレームから更新を掛けられないようにするため）
		transform.dirty = false;
	}
}