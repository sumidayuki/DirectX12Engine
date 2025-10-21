#pragma once

class TitleScene : public Scene
{
private:
	float m_time;

public:
	bool Load() override;
	void Start() override;
	void Update() override;
	void Draw() override;
};