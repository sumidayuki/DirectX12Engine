#pragma once

class UIEventSystem : public System
{
private:
	// マウスポインターのレイキャスト処理
	void RaycastPointer(World& world, EventSystem* eventState);
	
	// コントローラーのナビゲーション処理
	void ProcessControllerNavigation(World& world, EventSystem* eventState);

public:
	void Start(World& world) override;
	void Update(World& world) override;
};