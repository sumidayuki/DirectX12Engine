#pragma once

class World;

/// <summary>
/// Transformの管理を行うシステムです。
/// Transformコンポーネントの変更をMatrix等に反映します。
/// </summary>
class TransformSystem : public System
{
private:
	void UpdateAllDirtyTransforms(World& world);

public:
	void Start(World& world) override;
	void Update(World& world) override;
};