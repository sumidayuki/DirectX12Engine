#include "PlayerActionGuideSystem.h"
#include "PlayerTag.h"
#include "CharacterImporter.h"

void PlayerActionGuideSystem::Start(World& world)
{
	// 入力のバインドによるスプライトのマップを更新
	InputManager::UpdateSpriteBindMap();
}

void PlayerActionGuideSystem::Update(World& world)
{
	View<PlayerTag, MoveState, GuardState, Animator> view(world);

	for (auto [entity, player, moveState, guardState, animator] : view)
	{
		// まず全てのガイドを非表示にする
		SetEnableAll(false, world);

		// キャラクター情報を取得
		const CharacterInfo* info = CharacterInfoRegistry::GetInstance()->GetCharacterInfo(moveState.name);
		if (!info)
		{
			continue;
		}

		// 現在の入力デバイスを取得
		InputDeviceType deviceType = InputManager::GetCurrentInputDeviceType();

		const MoveData& currentMove = CharacterInfoRegistry::GetInstance()->GetMoveById(moveState.name, moveState.currentMoveId);

		for (const auto& transition : currentMove.transitions)
		{
			// 入力が押されたときの遷移のみを対象とする
			if (transition.type != MoveTransitionType::InputPressed)
			{
				continue;
			}

			const MoveData& nextMove = CharacterInfoRegistry::GetInstance()->GetMoveById(moveState.name, transition.targetMoveId);

			// 次のMoveに対応するガイドテキストが設定されていない場合はスキップ
			if (nextMove.textName.empty())
			{
				continue;
			}

			// ガイドテキストのUIオブジェクトを取得
			Entity text = UIManager::GetInstance()->GetUIObject(HashString("MainSceneUI"), HashString(nextMove.textName.c_str()));

			UIGraphic* textUI = world.GetComponent<UIGraphic>(text);
			if (!textUI)
			{
				continue;
			}

			textUI->isEnabled = true;

			Transform* textTransform = world.GetComponent<Transform>(text);
			if (!textTransform)
			{
				continue;
			}

			Transform* child = TransformAPI::GetChild(textTransform, 0);
			if (!child)
			{
				continue;
			}

			UIGraphic* childUI = world.GetComponent<UIGraphic>(child->entity);
			Image* childImage = world.GetComponent<Image>(child->entity);

			if (childImage)
			{
				childImage->sprite = InputManager::GetInputBind().GetSpriteBindMap().at(transition.inputKey).at(deviceType);
			}

			if (childUI)
			{
				childUI->isEnabled = true;
			}
		}
	}
}

void PlayerActionGuideSystem::SetEnableAll(bool isEnable, World& world)
{
	Entity guideEntity = UIManager::GetInstance()->GetUIObject(HashString("MainSceneUI"), HashString("PlayerActionGuide"));

	Transform* guideT = world.GetComponent<Transform>(guideEntity);
	if (!guideT)
	{
		return;
	}

	for (int i = 0; i < TransformAPI::GetChildCount(guideT); i++)
	{
		Transform* childT = TransformAPI::GetChild(guideT, i);
		if (!childT)
		{
			continue;
		}

		UIGraphic* childUI = world.GetComponent<UIGraphic>(childT->entity);

		if (childUI)
		{
			childUI->isEnabled = isEnable;
		}

		Transform* child = TransformAPI::GetChild(childT, 0);
		if (!child)
		{
			continue;
		}

		UIGraphic* childUI2 = world.GetComponent<UIGraphic>(child->entity);

		if (childUI2)
		{
			childUI2->isEnabled = isEnable;
		}
	}
}