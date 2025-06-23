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
		// �O��̍U���p�^�[���ɂ���Ď���̍U���p�^�[����I��
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
		// �̗͂� 0 �ȉ��Ȃ�
		if (status.life <= 0)
		{
			// �V�[�����^�C�g���V�[���ɕύX����B������Q�[���I�[�o�[������ǉ����܂��B
			SceneManager::ChangeScene("Title");
		}

		// y����0��菬�����ꍇ�́A���������Ɉړ�������B
		// ����ɂ���Ă������ƃ{�X���o�ꂵ�Ă��鉉�o���ł���B
		if (transform.position.y < 0)
		{
			m_transformSystem->Translate(transform, Vector3::down * 40 * Time::GetDeltaTime());
		}
		// �o�ꂪ�I�������
		else
		{
			// �_���[�W���󂯂��Ȃ�
			if (status.isDamaged)
			{
				// �_���[�W���������s����B
				Damage(status, renderCommand, boss);
			}

			// �e��^�C�}�[�ϐ��� deltaTime �����Z����B
			boss.patternTimer += Time::GetDeltaTime();
			boss.coolDownTimer += Time::GetDeltaTime();

			// �N�[���_�E���^�C�}�[���N�[���_�E���𒴂����Ȃ�
			if (boss.coolDownTimer > boss.coolDown)
			{
				// ���݂̍U���p�^�[���ɂ���Ċe�U�����s���B
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

			// �U���p�^�[���̃^�C�}�[���U���p�^�[���̎��s���Ԃ𒴂����Ȃ�
			if (boss.patternTimer >= boss.patternDuration)
			{
				// �^�C�}�[�� 0 �ɖ߂��܂��B
				boss.patternTimer = 0;

				// ���̍U���p�^�[�����擾���Đݒ肵�܂��B
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
