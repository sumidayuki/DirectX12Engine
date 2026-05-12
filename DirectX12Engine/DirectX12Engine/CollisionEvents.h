#pragma once

/// <summary>
/// コライダー同士が衝突した際に得られる情報を格納する構造体です。
/// </summary>
struct ContactInfo
{
	Entity other = INVALID_ENTITY;
	Vector3 point = Vector3::zero;  // 衝突点
	Vector3 normal = Vector3::zero; // 衝突面の法線ベクトル
	float penetration = 0.0f;       // 貫通深度
};

struct CollisionEvents : IComponentData
{
	static constexpr int MAX_CONTACTS = 8; // 最大接触点数

	ContactInfo enterContacts[MAX_CONTACTS]; // 衝突開始時の接触点情報
	ContactInfo stayContacts[MAX_CONTACTS];  // 衝突継続中の接触点情報
	ContactInfo exitContacts[MAX_CONTACTS];  // 衝突終了時の接触点情報

	int enterCount = 0; // 衝突開始時の接触点数
	int stayCount = 0;  // 衝突継続中の接触点数
	int exitCount = 0;  // 衝突終了時の接触点数
};