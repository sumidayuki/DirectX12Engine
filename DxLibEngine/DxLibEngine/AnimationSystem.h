#pragma once

/// <summary>
/// Animatorコンポーネントを処理し、ボーンの交換を更新するシステム。
/// </summary>
class AnimationSystem : public System
{
public:
	void Update(ComponentManager& cm, World& world) override;
};