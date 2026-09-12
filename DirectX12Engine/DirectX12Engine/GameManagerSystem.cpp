#include "GameManagerSystem.h"
#include "PlayerTag.h"
#include "BattleCamera.h"
#include "Enemy.h"
#include "CharacterImporter.h"
#include "PlayerCamera.h"
#include "ProceduralSkyRenderer.h"

void GameManagerSystem::NotifyDeath(Entity character, World& world)
{
	Time::SetTimeScale(0.0f);

	m_resultCanvas.enabled = true;

	Entity text = UIManager::GetInstance()->GetUIObject(HashString("ResultUI"), HashString("ResultText"));
	UIGraphic* textUI = world.GetComponent<UIGraphic>(text);
	Transform* textTransform = world.GetComponent<Transform>(text);
	Image* image = world.GetComponent<Image>(textTransform->entity);
	if (m_player == character)
	{
		Texture2D* texture = AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/Images/Text/text_lose.png");
		Rect spriteRect = { 0, 0, (float)texture->GetWidth(), (float)texture->GetHeight() };
		Sprite* newSprite = Sprite::Create(
			texture,
			spriteRect,
			Vector2(0.5f, 0.5f),
			1.0f,
			1.0f
		);
		image->sprite = newSprite;
	}
	else
	{
		Texture2D* texture = AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/Images/Text/text_win.png");
		Rect spriteRect = { 0, 0, (float)texture->GetWidth(), (float)texture->GetHeight() };
		Sprite* newSprite = Sprite::Create(
			texture,
			spriteRect,
			Vector2(0.5f, 0.5f),
			1.0f,
			1.0f
		);
		image->sprite = newSprite;
	}
}

bool GameManagerSystem::Load(World& world)
{
	m_enemy = INVALID_ENTITY;
	m_player = INVALID_ENTITY;

	return true;
}

void GameManagerSystem::Start(World& world)
{
	// プレイヤーを生成
	Entity player = world.CreateWithModel(L"Assets/Archer.fbx", nullptr, Vector3(0, 0, -500), Quaternion::identity, Layers::Player);
	world.AddComponent<Input>(player, Input{});
	world.AddComponent<PlayerTag>(player, PlayerTag{});
	world.AddComponent<LocomotionData>(player, LocomotionData{});
	RollingState rolling;
	rolling.direction = Vector3::zero;
	world.AddComponent<RollingState>(player, rolling);
	CharacterInfoRegistry::GetInstance()->CharcterInitialize("Archer", player, world);

	m_player = player;

	// 敵を生成
	Entity warrok = world.CreateWithModel(L"Assets/Warrok.fbx", nullptr, Vector3::zero, Quaternion::Euler(0, 180, 0), Layers::Enemy);
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
	std::string stateName = StatusAPI::GetString(CharacterInfoRegistry::GetInstance()->GetCharacterInfo("Warrok")->status, "aiCurrentState");
	aiState.currentStateID = FNV1a_Hash<uint32_t>(stateName);
	aiState.nextStateID = aiState.currentStateID;
	aiState.stateTime = 0;
	world.AddComponent<AIState>(warrok, aiState);
	AITrigger trigger;
	trigger.triggers = {};
	world.AddComponent<AITrigger>(warrok, trigger);
	CharacterInfoRegistry::GetInstance()->CharcterInitialize("Warrok", warrok, world);
	world.GetComponent<Collider>(warrok)->offset = Vector3(0, 180.0f, 0);

	m_enemy = warrok;

	//  バトルカメラの設定
	float fov = 60.0f;
	float aspect = (float)Screen::GetWidth() / (float)Screen::GetHeight();
	float nearPlane = 0.1f;
	float farPlane = 5000.0f;

	// バトルカメラの作成
	Entity cameraEntity = world.CreateCamera3D(fov, aspect, nearPlane, farPlane, CameraClearFlags::ProceduralSky);
	ProceduralSkyRenderer::Initialize(1.0f, 60.0f, false);

	world.AddComponent<AudioListener>(cameraEntity, AudioListener{});

	PlayerCamera playerCamera;
	playerCamera.player = player;
	playerCamera.offset = Vector3(-20, 200, -300);
	playerCamera.sensitivity = 0.1;
	world.AddComponent<PlayerCamera>(cameraEntity, playerCamera);
}

void GameManagerSystem::Update(World& world)
{
	if (!world.IsAlive(m_player))
	{

	}

	if (!world.IsAlive(m_enemy))
	{

	}
}