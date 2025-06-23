#include "EnemySystem.h"

void EnemySystem::Start(ComponentManager& cm, World& world)
{
	m_transformSystem = world.GetSystem<TransformSystem>();
}

void EnemySystem::Update(ComponentManager& cm, World& world)
{
	View<Transform, Enemy, CircleCollider2D, Status> view(cm);

	for (auto [entity, transform, enemy, coll, status] : view)
	{
		// entityが生きていない場合スキップする（バグ回避のため）
		if (!world.IsAlive(entity)) continue;

		// life が 0 以下だったらこのentityを破壊する
		if (status.life <= 0 || transform.position.y < -1000 || transform.position.y > Screen::GetHeight() + 1000 || transform.position.x < -1000 || transform.position.x > Screen::GetWidth() + 1000)
		{
			world.DestoryEntity(entity);
		}

		enemy.lifeTime += Time::GetDeltaTime();
		
		// enemyの隊列情報を基に分岐します。
		switch (enemy.formation)
		{
		case Formation::Standard:
		case Formation::V:
		case Formation::Wave:
			m_transformSystem->Translate(transform, Vector3::down * 200 * Time::GetDeltaTime());	// 下に進む
			break;

		case Formation::Circle:
			float speed = 10.0f;			// 進む速度
			float radius = 900.0f;		// 円運動の円の大きさ
			float rotateSpeed = 1.5f;	// 回転速度

			float angle = rotateSpeed * enemy.lifeTime;
			float offsetX = Mathf::Cos(angle) * radius;
			float offsetY = Mathf::Sin(angle) * radius;
			float baseX = speed * enemy.lifeTime;

			Vector3 axis = Vector3::forward;
			//m_transformSystem->Rotate(transform, axis, angle);
			m_transformSystem->Translate(transform, Vector3(baseX + offsetX, offsetY, 0) * Time::GetDeltaTime());

			break;
		}

		if (coll.info.state == CollisionState::Enter || coll.info.state == CollisionState::Stay)
		{
			if (world.HasComponent<Player>(coll.info.other))
			{
				if (!world.IsAlive(coll.info.other))continue;
				Status* status = world.GetComponent<Status>(coll.info.other);
				status->life -= 1;
				status->isDamaged = true;
				world.DestoryEntity(entity);
			}
		}
	}
}