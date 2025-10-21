#pragma once

class MainScene : public Scene
{
public:
	bool Load() override;
	void Start() override;
	void Update() override;
	void Draw() override;
};