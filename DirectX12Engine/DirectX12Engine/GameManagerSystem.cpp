#include "GameManagerSystem.h"
#include "PlayerTag.h"
#include "BattleCamera.h"
#include "Enemy.h"
#include "CharacterImporter.h"

bool GameManagerSystem::Load(World& world)
{
	m_enemy = INVALID_ENTITY;
	m_player = INVALID_ENTITY;

	return true;
}

void GameManagerSystem::Start(World& world)
{
	// プレイヤーを生成
	Entity player = world.CreateWithModel(L"Assets/player-01.fbx", nullptr, Vector3(0, 0, -500), Quaternion::identity);
	world.AddComponent<Input>(player, Input{});
	world.AddComponent<PlayerTag>(player, PlayerTag{});
	CharacterImporter::GetInstance()->CharcterInitialize("Archer", player, world);

	m_player = player;

	// 敵を生成
	Entity warrok = world.CreateWithModel(L"Assets/Warrok-00.fbx", nullptr, Vector3::zero, Quaternion::Euler(0, 180, 0));
	Transform* warrokT = world.GetComponent<Transform>(warrok);
	warrokT->scale = warrokT->scale * 1.5f;
	Enemy enemy;
	enemy.attackCoolDown = 0.0f;
	enemy.target = player;
	world.AddComponent<Enemy>(warrok, enemy);
	AIAgent agent;
	agent.speed = 175.0f;
	agent.acceleration = 10.0f;
	world.AddComponent<AIAgent>(warrok, agent);
	CharacterImporter::GetInstance()->CharcterInitialize("Warrok", warrok, world);
	world.GetComponent<Collider>(warrok)->offset = Vector3(0, 180.0f, 0);

	m_enemy = warrok;

	//  バトルカメラの設定
	float fov = 60.0f;
	float aspect = (float)Screen::GetWidth() / (float)Screen::GetHeight();
	float nearPlane = 0.1f;
	float farPlane = 5000.0f;

	// バトルカメラの作成
	Entity parent = world.CreateCamera3D(fov, aspect, nearPlane, farPlane);

	//Entity skybox = world.CreateSphere(1000.0f, 16, 16);
	//Transform* skyboxT = world.GetComponent<Transform>(skybox);
	//TransformSystem::GetInstance()->SetParent(*skyboxT, parentT);

	BattleCamera battleCamera;
	battleCamera.character1 = player;
	battleCamera.character2 = warrok;
	world.AddComponent<BattleCamera>(parent, battleCamera);

}

void GameManagerSystem::Update(World& world)
{
}
