#include "GameManagerSystem.h"
#include "PlayerTag.h"
#include "PlayerCamera.h"
#include "Enemy.h"

bool GameManagerSystem::Load(World& world)
{
	// プレイヤーを生成
	Entity player = world.CreateWithModel(L"Assets/player.fbx", nullptr, Vector3(0, 0, -500), Quaternion::identity);
	world.AddComponent<PlayerTag>(player, PlayerTag{});
	world.AddComponent<Input>(player, Input{});
	world.AddComponent<Velocity>(player, Velocity{});
	
	// プレイヤーカメラの設定
	float fov = 60.0f;
	float aspect = (float)Screen::GetWidth() / (float)Screen::GetHeight();
	float nearPlane = 0.1f;
	float farPlane = 5000.0f;
	Vector3 cameraPos = Vector3(6.0f, 1.0f, -8.0f);
	
	// プレイヤーカメラの作成
	Entity parent = world.CreateEntity();
	Transform* parentT = world.GetComponent<Transform>(parent);
	Transform* cameraT = world.GetComponent<Transform>(world.CreateCamera3D(fov, aspect, nearPlane, farPlane, cameraPos));
	TransformSystem::GetInstance()->SetParent(*cameraT, parentT);
	TransformSystem::GetInstance()->SetLocalPosition(*cameraT, Vector3::forward * 20.0f);

	//Entity skybox = world.CreateSphere(1000.0f, 16, 16);
	//Transform* skyboxT = world.GetComponent<Transform>(skybox);
	//TransformSystem::GetInstance()->SetParent(*skyboxT, parentT);

	world.AddComponent<PlayerCamera>(parent, PlayerCamera{ .player = player, .offset = Vector3(0, 150, -300), .sensitivity = 0.5f });
	
	// 敵を生成
	Entity warrok = world.CreateWithModel(L"Assets/Warrok-00.fbx", nullptr, Vector3::zero, Quaternion::Euler(0, 180, 0));
	Transform* transform = world.GetComponent<Transform>(warrok);
	transform->scale = transform->scale * 1.5f;
	Enemy enemy;
	enemy.target = player;
	world.AddComponent<Enemy>(warrok, enemy);
	AIAgent agent;
	agent.speed = 100.0f;
	agent.acceleration = 10.0f;
	world.AddComponent<AIAgent>(warrok, agent);	
	AABBCollider coll;
	coll.bounds = Bounds(Vector3(0, 0, 0), Vector3(90, 240, 90));
	coll.offset = Vector3(0, 120, 0);
	world.AddComponent<AABBCollider>(warrok, coll);

	return true;
}

void GameManagerSystem::Start(World& world)
{


}

void GameManagerSystem::Update(World& world)
{
}
