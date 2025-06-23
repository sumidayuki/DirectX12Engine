#pragma once

class TitleScene : public Scene
{
private:
	float m_time;

public:
	void Start() override;
	void Update() override;
	void Draw() override;
};