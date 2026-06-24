#include "PlayerActionGuideSystem.h"
#include "PlayerTag.h"
#include "CharacterImporter.h"

void PlayerActionGuideSystem::Start(World& world)
{
}

void PlayerActionGuideSystem::Update(World& world)
{
	View<PlayerTag, ComboState, GuardState, Animator> view(world);

	for (auto [entity, player, comboState, guardState, animator] : view)
	{
		SetEnableAll(false, world);

		if (guardState.isGuarding)
		{
			Entity text = UIManager::GetInstance()->GetUIObject(HashString("MainSceneUI"), HashString("text_guard-attack"));
			UIGraphic* textUI = world.GetComponent<UIGraphic>(text);
			textUI->isEnabled = true;

			continue;
		}

		const CharacterInfo* info = CharacterImporter::GetInstance()->GetCharacterInfo(comboState.name);

		if (comboState.currentMoveId == 0)
		{
			for (const auto& move : info->moves)
			{
				if (move.isStarter)
				{
					Entity text = UIManager::GetInstance()->GetUIObject(HashString("MainSceneUI"), HashString(move.textName.c_str()));
					UIGraphic* textUI = world.GetComponent<UIGraphic>(text);
					textUI->isEnabled = true;
				}
			}

			{
				Entity text = UIManager::GetInstance()->GetUIObject(HashString("MainSceneUI"), HashString("text_guard-attack"));
				UIGraphic* textUI = world.GetComponent<UIGraphic>(text);
				textUI->isEnabled = false;
			}

			{
				Entity text = UIManager::GetInstance()->GetUIObject(HashString("MainSceneUI"), HashString("text_rolling"));
				UIGraphic* textUI = world.GetComponent<UIGraphic>(text);
				textUI->isEnabled = true;
			}
			{
				Entity text = UIManager::GetInstance()->GetUIObject(HashString("MainSceneUI"), HashString("text_guard"));
				UIGraphic* textUI = world.GetComponent<UIGraphic>(text);
				textUI->isEnabled = true;
			}

			continue;
		}
		else
		{
			const ComboMove& currentMove = CharacterImporter::GetInstance()->GetMoveById(comboState.name, comboState.currentMoveId);

			for (const auto& move : currentMove.nextPossibleMoves)
			{
				const ComboMove& nextMove = CharacterImporter::GetInstance()->GetMoveById(comboState.name, move);
				Entity text = UIManager::GetInstance()->GetUIObject(HashString("MainSceneUI"), HashString(nextMove.textName.c_str()));
				UIGraphic* textUI = world.GetComponent<UIGraphic>(text);
				textUI->isEnabled = true;
			}

			continue;
		}
	}
}

void PlayerActionGuideSystem::SetEnableAll(bool isEnable, World& world)
{
	Entity guideEntity = UIManager::GetInstance()->GetUIObject(HashString("MainSceneUI"), HashString("PlayerActionGuide"));
	Transform* guideT = world.GetComponent<Transform>(guideEntity);

	for (int i = 0; i < TransformSystem::GetInstance()->GetChildCount(guideT); i++)
	{
		Transform* childT = TransformSystem::GetInstance()->GetChild(guideT, i);
		UIGraphic* childUI = world.GetComponent<UIGraphic>(childT->entity);
		childUI->isEnabled = isEnable;
	}

}
