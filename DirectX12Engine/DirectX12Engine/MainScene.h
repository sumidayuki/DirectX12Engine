#pragma once

class MainScene : public Scene
{
private:
	Entity* m_model;
	Animator* m_animator = nullptr;
	Animation* m_tposeAnim = nullptr;
	Animation* m_attackAnim = nullptr;

public:
	void Start() override;
	void Update() override;
	void Draw() override;
};