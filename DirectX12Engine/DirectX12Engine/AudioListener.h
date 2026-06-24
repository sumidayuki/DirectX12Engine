#pragma once

/// <summary>
/// オーディオリスナーコンポーネント。これを持つエンティティは、AudioSystemによってリスナーとして扱われます。
/// シーン内にある最初の1つをリスナーとして扱います。
/// </summary>
struct AudioListener : IComponentData
{
	bool enabled = true; // オーディオリスナーが有効かどうか
};