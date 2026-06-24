#include "TitleScene.h"
#include "SystemList.h"
using namespace UIAPI;

bool TitleScene::Load()
{
	m_world.AddSystem(std::make_unique<InputSystem>());

	AssetManager::GetInstance()->LoadAsset(AssetType::Texture, L"Assets/White.png");
	AssetManager::GetInstance()->LoadAsset(AssetType::Audio, L"Assets/Audio/BGM/title.wav");

	return true;
}

void TitleScene::Start()
{
	// カメラの作成
	float fov = 60.0f;
	float aspect = (float)Screen::GetWidth() / (float)Screen::GetHeight();
	float nearPlane = 0.1f;
	float farPlane = 5000.0f;

	Entity camera = m_world.CreateCamera3D(fov, aspect, nearPlane, farPlane, Color::black, Vector3(0, 0, -10));
	Entity background = m_world.CreateWithSprite(L"Assets/Title.png", Rect(0, 0, 1920, 1080), Vector2(0.5f, 0.5f), 84);

	Entity text = m_world.CreateWithSprite(L"Assets/Title_Text.png", Rect(0, 0, 971, 125), Vector2(0.5f, 0.5f), 84, nullptr, Vector3(0, -2, 0));
	m_titleText = m_world.GetComponent<SpriteRenderer>(text);

	m_world.AddComponent<AudioListener>(camera, AudioListener{});

	AudioClip* bgm = AssetManager::GetInstance()->GetAsset<AudioClip>(AssetType::Audio, L"Assets/Audio/BGM/title.wav");
	AudioSource bgmSource;
	bgmSource.clip = bgm;
	bgmSource.playOnAwake = true;         // 自動再生
	bgmSource.volume = 0.5f;
	bgmSource.spatialBlend = 0.0f;        // 2D（距離減衰なし）
	bgmSource.loop = true;
	m_world.AddComponent<AudioSource>(m_world.CreateEntity("TitleBGM"), bgmSource);
}

void TitleScene::Update()
{
	// タイトルテキストの点滅
	float alpha = (std::sin(Time::GetTime() * 3.0f) + 1.0f) / 2.0f; // 0から1の範囲で変化
	m_titleText->color.a = alpha;

	if (InputManager::IsAnyKeyPressed())
	{
		SceneManager::ChangeScene("Main");
	}
}

void TitleScene::Draw()
{
}
