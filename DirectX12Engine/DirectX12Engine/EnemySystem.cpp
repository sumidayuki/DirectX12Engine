#include "Precompiled.h"
#include "EnemyTag.h"

void EnemySystem::Start(ComponentManager& cm, World& world)
{

}

void EnemySystem::Update(ComponentManager& cm, World& world)
{
	View<EnemyTag, Transform, Animator> view(cm);

	for (auto [entity, enemyTag, transform, animator] : view)
	{
		AnimationSystem::Play(animator, "Idle");
	}
}
