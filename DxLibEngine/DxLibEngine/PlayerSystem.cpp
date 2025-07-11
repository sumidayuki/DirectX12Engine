#include "PlayerSystem.h"

void PlayerSystem::MovePlayer(Input& input, Transform& transform, Velocity& velocity)
{
	Vector2 direction = Vector2();

	if (input.moveLeft)
	{
		if (transform.position.x >= 0)
		{
			direction.x -= 1;
		}
	}
	if (input.moveRight)
	{
		if (transform.position.x <= Screen::GetWidth())
		{
			direction.x += 1;
		}
	}
	if (input.moveUp)
	{
		if (transform.position.y <= Screen::GetHeight())
		{
			direction.y += 1;
		}
	}
	if (input.moveDown)
	{
		if (transform.position.y >= 0)
		{
			direction.y -= 1;
		}
	}

	if (direction.Magnitude() > 0)
	{
		direction = direction.Normalized();
		m_transformSystem->Translate(transform, Vector3(direction * velocity.speed * Time::GetDeltaTime(), 0));
	}
}

void PlayerSystem::Start(ComponentManager& cm, World& world)
{
	m_transformSystem = world.GetSystem<TransformSystem>();

	TextureImporter importer;
	m_bulletTex = importer.Import(L"Assets/images/bullet_01.png");

	// プレイヤーを作成
	Entity* player = world.CreateWithSprite(L"Assets/images/bc-01.png", Rect(0, 0, 128, 128), Vector2(0.5f, 0.5f), 1.0f, nullptr, Vector3(Screen::GetWidth() / 2, 300, 40));
	world.AddComponent<Player>(*player, Player{});
	world.AddComponent<Input>(*player, Input{});
	world.AddComponent<Velocity>(*player, Velocity{ .speed = 350 });
	world.AddComponent<Status>(*player, Status{ 3 });
	world.AddComponent<CircleCollider2D>(*player, CircleCollider2D{ .radius = 40 });

	for (int i = 0; i < 3; i++)
	{
		Entity* e = world.CreateEntity();
		Transform* transform = world.GetComponent<Transform>(*e);
		transform->position = Vector3(500 - i * 150, 900, 0);
		transform->scale = Vector3(0.1f, 0.1f, 0);
		transform->dirty = true;

		//m_lifeObjects.push(e);
	}
}

void PlayerSystem::Update(ComponentManager& cm, World& world)
{
	// Viewクラスを使用して world 内の各コンポーネントを全て所持している Entity を探します。
	View<Player, Input, Velocity, Transform, Status> view(cm);

	for (auto [entity, player, input, velocity, transform, status] : view)
	{
		if (status.life <= 0)
		{
			world.DestoryEntity(entity);
		}

		// ダメージを受けたなら
		if (status.isDamaged)
		{
			player.damageTimer = 1.0f;

			status.isDamaged = false;

			//.sprite.g = 0;
			//renderCommand.sprite.b = 0;

			//world.DestoryEntity(m_lifeObjects.front());

			//m_lifeObjects.pop();
		}

		if (player.damageTimer >= 0)
		{
			player.damageTimer -= 1.0f * Time::GetDeltaTime();
		}
		else
		{
			//.sprite.g = 255;
			//renderCommand.sprite.b = 255;
		}

		MovePlayer(input, transform, velocity);

		m_timer += Time::GetDeltaTime();

		if (input.shot)
		{
			if (m_timer >= m_coolDown)
			{
				Entity* bullet = world.CreateWithSprite(m_bulletTex, Rect(0, 0, 64, 64), Vector2(0.5f, 0.5f), 1.0f, nullptr, transform.position + (Vector3::up * 50) + (Vector3::forward * 15));
				Transform* bTransform = world.GetComponent<Transform>(*bullet);
				world.AddComponent<Bullet>(*bullet, Bullet{ .damage = 3 });
				world.AddComponent<Velocity>(*bullet, Velocity{ .speed = 700 });
				world.AddComponent<CircleCollider2D>(*bullet, CircleCollider2D{ .radius = 10 });			

				m_timer = 0;
			}
		}
	}
}
