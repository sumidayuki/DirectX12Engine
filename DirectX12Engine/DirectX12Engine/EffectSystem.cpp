#include "EffectSystem.h"
#include "EffectManager.h"

Effekseer::Matrix44 ToEffekseerMatrix(const Matrix4x4& matrix)
{
	Effekseer::Matrix44 result;

	for (int row = 0; row < 4; ++row)
	{
		for (int column = 0; column < 4; ++column)
		{
			result.Values[row][column] = matrix.m[row][column];
		}
	}

	return result;
}

Effekseer::Matrix43 ToEffekseerMatrix43(const Matrix4x4& matrix)
{
	Effekseer::Matrix43 result;

	result.Value[0][0] = matrix._11;
	result.Value[0][1] = matrix._12;
	result.Value[0][2] = matrix._13;

	result.Value[1][0] = matrix._21;
	result.Value[1][1] = matrix._22;
	result.Value[1][2] = matrix._23;

	result.Value[2][0] = matrix._31;
	result.Value[2][1] = matrix._32;
	result.Value[2][2] = matrix._33;

	result.Value[3][0] = matrix._41;
	result.Value[3][1] = matrix._42;
	result.Value[3][2] = matrix._43;

	return result;
}

void EffectSystem::Update(World& world)
{
	EffectManager* em = EffectManager::GetInstance();

	View<EffectSource, Transform> view(world);

	for (auto [entity, source, transform] : view)
	{
		// PlayOnAwake
		if (source.playOnAwake && source.effect && !source.isStarted)
		{
			source.isStarted = true;
			source.requestPlay = true;
		}

		// Stop要求
		if (source.requestStop)
		{
			source.requestStop = false;
			StopEffect(source);
		}

		// Play要求
		if (source.requestPlay)
		{
			source.requestPlay = false;

			if (source.effect)
			{
				// 再生中なら一旦停止
				if (source.isPlaying)
				{
					StopEffect(source);
				}

				StartEffect(source, transform);
			}
		}

		// 再生中
		if (source.isPlaying && source.handle >= 0)
		{
			// 再生終了確認
			if (!em->GetManager()->Exists(source.handle))
			{
				source.isPlaying = false;
				source.handle = -1;

				if(source.loop)
				{
					source.requestPlay = true;
				}

				continue;
			}

			if (source.lifeTime <= 0.0f)
			{
				StopEffect(source);

				if (source.loop)
				{
					source.requestPlay = true;
				}

				continue;
			}

			// Transform追従
			if (source.followTransform)
			{
				const Effekseer::Matrix43 matrix = ToEffekseerMatrix43(transform.localToWorldMatrix);
				em->GetManager()->SetMatrix(source.handle, matrix);
			}

			// スピード
			em->GetManager()->SetSpeed(source.handle, source.speed);

			// カラー
			Effekseer::Color color(
				static_cast<uint8_t>((std::clamp(source.color.r, 0.0f, 1.0f) * 255.0f)), 
				static_cast<uint8_t>((std::clamp(source.color.g, 0.0f, 1.0f) * 255.0f)), 
				static_cast<uint8_t>((std::clamp(source.color.b, 0.0f, 1.0f) * 255.0f)),
				static_cast<uint8_t>((std::clamp(source.color.a, 0.0f, 1.0f) * 255.0f))
			);
			em->GetManager()->SetAllColor(source.handle, color);

			// 生存時間
			source.lifeTime -= Time::GetDeltaTime();
		}
	}

	// OneShotEffectの後始末
	std::vector<Entity> toDestroy;

	View<EffectSource> cleanupView(world);

	for (auto [entity, source] : cleanupView)
	{
		if (entity.name == "OneShotEffect" && !source.isPlaying && source.isStarted && !source.loop)
		{
			toDestroy.push_back(entity);
		}
	}

	for (const Entity& entity : toDestroy)
	{
		world.DestroyEntity(entity);
	}

	em->GetManager()->Update(Time::GetDeltaTime() * 60.0f);
}

void EffectSystem::Draw(World& world)
{
	EffectManager* em = EffectManager::GetInstance();

	//Camera
	CameraSystem* cameraSystem = world.GetSystem<CameraSystem>();
	if (!cameraSystem)
	{
		return;
	}

	Camera* camera = cameraSystem->GetCurrent();
	if (!camera)
	{
		return;
	}

	Entity cameraEntity = cameraSystem->GetCurrentEntity();
	Transform* cameraTransform = world.GetComponent<Transform>(cameraEntity);

	// View Matrix
	const Matrix4x4& viewMatrix = cameraSystem->GetWorldToCameraMatrix(*cameraTransform, world);

	// Projection Matrix
	const Matrix4x4& projectionMatrix = cameraSystem->GetProjectionMatrix(*camera);

	// Effekseerへ設定
	em->GetRenderer()->SetCameraMatrix(ToEffekseerMatrix(viewMatrix));
	em->GetRenderer()->SetProjectionMatrix(ToEffekseerMatrix(projectionMatrix));

	// Draw
	FrameResource* frameResource = Graphics::GetCurrentFrameResource();
	ID3D12GraphicsCommandList* commandList = frameResource->GetCommandList();

	// Effeksserフレーム開始
	em->GetMemoryPool()->NewFrame();

	// DX12 CommandList接続
	EffekseerRendererDX12::BeginCommandList(em->GetCommandList(), commandList);

	// Effect描画
	em->GetRenderer()->BeginRendering();
	em->GetManager()->Draw();
	em->GetRenderer()->EndRendering();

	// CommandList終了
	EffekseerRendererDX12::EndCommandList(em->GetCommandList());
}

void EffectSystem::StartEffect(EffectSource& source, const Transform& transform)
{
	EffectManager* em = EffectManager::GetInstance();

	if (!source.effect)
	{
		return;
	}

	if (!source.effect->IsValid())
	{
		return;
	}

	source.handle = em->GetManager()->Play(source.effect->GetNativeEffect(), 0.0f, 0.0f, 0.0f);

	if (source.handle < 0)
	{
		return;
	}

	const Effekseer::Matrix43 matrix = ToEffekseerMatrix43(transform.localToWorldMatrix);

	Vector3 position = transform.position;

	em->GetManager()->SetMatrix(source.handle, matrix);
	em->GetManager()->SetSpeed(source.handle, source.speed);

	source.isPlaying = true;
	source.isStarted = true;
}

void EffectSystem::StopEffect(EffectSource& source)
{
	EffectManager* em = EffectManager::GetInstance();

	if (source.handle >= 0)
	{
		em->GetManager()->StopEffect(source.handle);
	}

	source.handle = -1;
	source.isPlaying = false;
}
