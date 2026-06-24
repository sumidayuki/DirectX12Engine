#include "GameManagerSystem.h"
#include "PlayerTag.h"
#include "BattleCamera.h"
#include "Enemy.h"
#include "CharacterImporter.h"
#include "PlayerCamera.h"

bool GameManagerSystem::Load(World& world)
{
	m_enemy = INVALID_ENTITY;
	m_player = INVALID_ENTITY;

	return true;
}

void GameManagerSystem::Start(World& world)
{
	// プレイヤーを生成
	Entity player = world.CreateWithModel(L"Assets/player-01.fbx", nullptr, Vector3(0, 0, -500), Quaternion::identity, Layers::Player);
	world.AddComponent<Input>(player, Input{});
	world.AddComponent<PlayerTag>(player, PlayerTag{});
	world.AddComponent<LocomotionData>(player, LocomotionData{});
	RollingState rolling;
	rolling.direction = Vector3::zero;
	rolling.duration = 1.63f;
	rolling.invincibleStart = 0.2f;
	rolling.invincibleEnd = 0.8f;
	rolling.speed = 350.0f;
	world.AddComponent<RollingState>(player, rolling);
	Stamina stamina;
	stamina.barName = "PlayerStaminaBar";
	stamina.maxValue = 100.0f;
	stamina.value = stamina.maxValue;
	world.AddComponent<Stamina>(player, stamina);
	CharacterImporter::GetInstance()->CharcterInitialize("Archer", player, world);

	m_player = player;

	// 敵を生成
	Entity warrok = world.CreateWithModel(L"Assets/Warrok-00.fbx", nullptr, Vector3::zero, Quaternion::Euler(0, 180, 0), Layers::Enemy);
	Transform* warrokT = world.GetComponent<Transform>(warrok);
	warrokT->scale = warrokT->scale * 1.5f;
	Enemy enemy;
	enemy.target = player;
	world.AddComponent<Enemy>(warrok, enemy);
	AIAgent agent;
	agent.speed = 175.0f;
	agent.acceleration = 10.0f;
	world.AddComponent<AIAgent>(warrok, agent);
	world.AddComponent<LocomotionData>(warrok, LocomotionData{});
	AIState aiState;
	std::string stateName = StatusAPI::GetString(CharacterImporter::GetInstance()->GetCharacterInfo("Warrok")->status, "aiCurrentState");
	aiState.currentStateID = FNV1a_Hash<uint32_t>(stateName);
	aiState.nextStateID = aiState.currentStateID;
	aiState.stateTime = 0;
	world.AddComponent<AIState>(warrok, aiState);
	AITrigger trigger;
	trigger.triggers = {};
	world.AddComponent<AITrigger>(warrok, trigger);
	CharacterImporter::GetInstance()->CharcterInitialize("Warrok", warrok, world);
	world.GetComponent<Collider>(warrok)->offset = Vector3(0, 180.0f, 0);

	m_enemy = warrok;

	//  バトルカメラの設定
	float fov = 60.0f;
	float aspect = (float)Screen::GetWidth() / (float)Screen::GetHeight();
	float nearPlane = 0.1f;
	float farPlane = 5000.0f;

	// バトルカメラの作成
	Entity cameraEntity = world.CreateCamera3D(fov, aspect, nearPlane, farPlane);

	//Entity skybox = world.CreateSphere(1000.0f, 16, 16);
	//Transform* skyboxT = world.GetComponent<Transform>(skybox);
	//TransformSystem::GetInstance()->SetParent(*skyboxT, parentT);

	PlayerCamera playerCamera;
	playerCamera.player = player;
	playerCamera.offset = Vector3(-20, 200, -300);
	playerCamera.sensitivity = 0.1;
	world.AddComponent<PlayerCamera>(cameraEntity, playerCamera);
}

void GameManagerSystem::Update(World& world)
{
}