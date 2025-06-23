#include "PlayerSystem.h"

void PlayerSystem::MovePlayer(Input& input, Transform& transform, Velocity& velocity)
{
	Vector3 direction = Vector3();

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
		if (transform.position.y >= 0)
		{
			direction.y -= 1;
		}
	}
	if (input.moveDown)
	{
		if (transform.position.y <= Screen::GetHeight())
		{
			direction.y += 1;
		}
	}

	if (direction.Magnitude() > 0)
	{
		direction = direction.Normalized();
		m_transformSystem->Translate(transform, direction * velocity.speed * Time::GetDeltaTime());
	}
}

void PlayerSystem::Start(ComponentManager& cm, World& world)
{
	m_transformSystem = world.GetSystem<TransformSystem>();

	// 通常の敵を事前に読み込んでおく
	Sprite heart;
	{
		int heartHandle = LoadGraph("Assets/heart.png");

		int w, h;
		GetGraphSize(heartHandle, &w, &h);

		heart = Sprite{ .handle = heartHandle, .width = w, .height = h };
	}

	for (int i = 0; i < 3; i++)
	{
		Entity e = world.CreateEntity();
		world.AddComponent<RenderCommand>(e, RenderCommand{ .layer = Layer::UI, .type = RenderType::Sprite, .sprite = heart });
		Transform* transform = world.GetComponent<Transform>(e);
		transform->position = Vector3(500 - i * 150, 900, 0);
		transform->scale = Vector3(0.1f, 0.1f, 0);
		transform->dirty = true;

		m_lifeObjects.push(e);
	}
}

void PlayerSystem::Update(ComponentManager& cm, World& world)
{
	// Viewクラスを使用して world 内の各コンポーネントを全て所持している Entity を探します。
	View<Player, Input, Velocity, Transform, Status, RenderCommand> view(cm);

	for (auto [entity, player, input, velocity, transform, status, renderCommand] : view)
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

			renderCommand.sprite.g = 0;
			renderCommand.sprite.b = 0;

			world.DestoryEntity(m_lifeObjects.front());

			m_lifeObjects.pop();
		}

		if (player.damageTimer >= 0)
		{
			player.damageTimer -= 1.0f * Time::GetDeltaTime();
		}
		else
		{
			renderCommand.sprite.g = 255;
			renderCommand.sprite.b = 255;
		}

		MovePlayer(input, transform, velocity);

		m_timer += Time::GetDeltaTime();

		if (input.shot)
		{
			if (m_timer >= m_coolDown)
			{
				Entity bullet = world.CreateEntity();
				Transform* bTransform = world.GetComponent<Transform>(bullet);

				// 現在の位置を 現在の位置 + 上に50 + 奥行65
				bTransform->position = transform.position + (Vector3::up * 50) + (Vector3::forward * 65);
				Vector3 axis = Vector3::forward;
				m_transformSystem->Rotate(*bTransform, axis, 180);
				world.AddComponent<Bullet>(bullet, Bullet{ .damage = 3 });
				world.AddComponent<Velocity>(bullet, Velocity{ .speed = 700 });
				world.AddComponent<RenderCommand>(bullet, RenderCommand{.layer = Layer::Bullet, .type = RenderType::Circle, .circle = Circle{.radius = 10 } });
				world.AddComponent<CircleCollider2D>(bullet, CircleCollider2D{ .radius = 10 });			

				m_timer = 0;
			}
		}
	}
}
