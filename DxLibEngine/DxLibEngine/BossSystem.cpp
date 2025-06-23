#include "BossSystem.h"

void BossSystem::GetNextPattern(Boss& boss, BossAttackPattern& current, BossAttackPattern& previous)
{
	if (current != BossAttackPattern::Wait)
	{
		boss.previousPattern = current;
		boss.currentPattern = BossAttackPattern::Wait;
	}
	else
	{
		// 前回の攻撃パターンによって次回の攻撃パターンを選択
		switch (previous)
		{
		case BossAttackPattern::AttackA:
			boss.currentPattern = BossAttackPattern::AttackB;
			break;
		case BossAttackPattern::AttackB:
			boss.currentPattern = BossAttackPattern::AttackC;
			break;
		case BossAttackPattern::AttackC:
			boss.currentPattern = BossAttackPattern::AttackA;
			break;
		}
	}
}

float BossSystem::GetNextDuration(BossAttackPattern current)
{
	switch (current)
	{
	case BossAttackPattern::AttackA:
		return 10.0f;
	case BossAttackPattern::AttackB:
		return 20.0f;
	case BossAttackPattern::AttackC:
		return 18.0f;
	case BossAttackPattern::Wait:
		return 3.0f;
	}

	return 6.0f;
}

float BossSystem::GetNextCoolDown(BossAttackPattern current)
{
	switch (current)
	{
	case BossAttackPattern::AttackA:
		return 1.0f;
	case BossAttackPattern::AttackB:
		return 0.0f;
	case BossAttackPattern::AttackC:
		return 8.0f;
	case BossAttackPattern::Wait:
		return 0.0f;
	}

	return 0.0f;
}

Vector3 BossSystem::GetNextPosition(BossAttackPattern& current, Transform& transform)
{
	int random = rand() % 100;

	switch (current)
	{
	case BossAttackPattern::AttackA:
		return Vector3(Screen::GetWidth() / 2, 0, 50);
	case BossAttackPattern::AttackB:
		return Vector3(Screen::GetWidth() / 2, 0, 50);
	case BossAttackPattern::AttackC:
		if (random < 33)
		{
			return Vector3(300, 0, 50);
		}
		else if (random < 66)
		{
			return Vector3(Screen::GetHeight() / 2, 0, 50);
		}
		else
		{
			return Vector3(1620, 0, 50);
		}
	case BossAttackPattern::Wait:
		return Vector3(Screen::GetWidth() / 2, 0, 50);
	}

	return Vector3(Screen::GetWidth() / 2, 0, 50);
}

void BossSystem::AttackA(World& world)
{
	for (int i = 0; i < 360; i++)
	{
		if (i % 30 == 0)
		{
			Entity bullet = world.CreateEntity();
			world.AddComponent<BossBullet>(bullet, BossBullet{ .damage = 1, .bulletType = BulletType::normal });
			world.AddComponent<Velocity>(bullet, Velocity{ .speed = 100 });
			world.AddComponent<RenderCommand>(bullet, RenderCommand{.layer = Layer::Bullet, .type = RenderType::Circle, .circle = Circle{.radius = 30, .r = 255, .g = 0, .b = 0} });
			world.AddComponent<CircleCollider2D>(bullet, CircleCollider2D{ .radius = 30 });
			Transform* bTransform = world.GetComponent<Transform>(bullet);
			m_transformSystem->Translate(*bTransform, Vector3(Screen::GetWidth() / 2, 300, 70));
			m_transformSystem->Rotate(*bTransform, Vector3::forward, i);
		}
	}
}

void BossSystem::AttackB(World& world, Transform& transform)
{
	static float phase = 0.0f;

	m_transformSystem->Translate(transform, Vector3(1.0f, 0, 0));
	transform.position = Vector3(960 + 600 * Mathf::Sin(phase), 0, 50);

	phase += 0.01f;
	
	static float timer = 0; 
	timer += Time::GetDeltaTime();

	if (timer > 0.5f)
	{
		Entity bullet = world.CreateEntity();
		world.AddComponent<BossBullet>(bullet, BossBullet{ .damage = 1, .bulletType = BulletType::normal });
		world.AddComponent<Velocity>(bullet, Velocity{ .speed = 100 });
		world.AddComponent<RenderCommand>(bullet, RenderCommand{ .layer = Layer::Bullet, .type = RenderType::Circle, .circle = Circle{.radius = 30, .r = 255, .g = 0, .b = 0} });
		world.AddComponent<CircleCollider2D>(bullet, CircleCollider2D{ .radius = 30 });
		Transform* bTransform = world.GetComponent<Transform>(bullet);
		m_transformSystem->Translate(*bTransform, Vector3(transform.position.x, 300, 70));
		timer = 0;
	}
}

void BossSystem::AttackC(World& world, Transform& transform)
{
	Entity bullet = world.CreateEntity();
	world.AddComponent<BossBullet>(bullet, BossBullet{ .damage = 1, .bulletType = BulletType::laser });
	world.AddComponent<Velocity>(bullet, Velocity{ .speed = 100 });
	world.AddComponent<RenderCommand>(bullet, RenderCommand{ .layer = Layer::Bullet, .type = RenderType::Box, .box = Box{.x = 500, .y = 1000, .r = 255, .g = 0, .b = 0} });
	world.AddComponent<BoxCollider2D>(bullet, BoxCollider2D{ .rect = Vector2(500, 1000 )});
	Transform* bTransform = world.GetComponent<Transform>(bullet);
	m_transformSystem->Translate(*bTransform, Vector3(transform.position.x - 250, 300, 70));
}

void BossSystem::Damage(Status& status, RenderCommand& renderCommand, Boss& boss)
{
	boss.damageTimer = 0.5f;

	status.isDamaged = false;

	renderCommand.sprite.g = 0;
	renderCommand.sprite.b = 0;
}

void BossSystem::Start(ComponentManager& cm, World& world)
{
	m_transformSystem = world.GetSystem<TransformSystem>();
}

void BossSystem::Update(ComponentManager& cm, World& world)
{
	View<Transform, Boss, Status, RenderCommand> view(cm);

	for (auto [entity, transform, boss, status, renderCommand] : view)
	{
		// 体力が 0 以下なら
		if (status.life <= 0)
		{
			// シーンをタイトルシーンに変更する。※今後ゲームオーバー処理を追加します。
			SceneManager::ChangeScene("Title");
		}

		// y軸が0より小さい場合は、少しずつ下に移動させる。
		// これによってゆっくりとボスが登場してくる演出ができる。
		if (transform.position.y < 0)
		{
			m_transformSystem->Translate(transform, Vector3::down * 40 * Time::GetDeltaTime());
		}
		// 登場が終わったら
		else
		{
			// ダメージを受けたなら
			if (status.isDamaged)
			{
				// ダメージ処理を実行する。
				Damage(status, renderCommand, boss);
			}

			// 各種タイマー変数に deltaTime を加算する。
			boss.patternTimer += Time::GetDeltaTime();
			boss.coolDownTimer += Time::GetDeltaTime();

			// クールダウンタイマーがクールダウンを超えたなら
			if (boss.coolDownTimer > boss.coolDown)
			{
				// 現在の攻撃パターンによって各攻撃を行う。
				switch (boss.currentPattern)
				{
				case BossAttackPattern::AttackA:
					AttackA(world);
					boss.coolDownTimer = 0.0f;
					break;

				case BossAttackPattern::AttackB:
					AttackB(world, transform);
					boss.coolDownTimer = 0.0f;
					break;

				case BossAttackPattern::AttackC:
					AttackC(world, transform);
					boss.coolDownTimer = 0.0f;
					break;

				case BossAttackPattern::Wait:
					break;
				}
			}

			// 攻撃パターンのタイマーが攻撃パターンの実行期間を超えたなら
			if (boss.patternTimer >= boss.patternDuration)
			{
				// タイマーを 0 に戻します。
				boss.patternTimer = 0;

				// 次の攻撃パターンを取得して設定します。
				GetNextPattern(boss, boss.currentPattern, boss.previousPattern);

				boss.patternDuration = GetNextDuration(boss.currentPattern);

				boss.coolDown = GetNextCoolDown(boss.currentPattern);

				transform.position = GetNextPosition(boss.currentPattern, transform);

				transform.dirty = true;
			}
		}

		if (boss.damageTimer >= 0)
		{
			boss.damageTimer -= 1.0f * Time::GetDeltaTime();
		}
		else
		{
			renderCommand.sprite.g = 255;
			renderCommand.sprite.b = 255;
		}
	}
}
