#pragma once

/// <summary>
/// エンティティを表す構造体です。
/// 第一成分：EntitySize(uint32_t)型の ID エンティティ番号を表します。
/// 第二成分：EntitySize(uint32_t)型の generation 世代番号を表します。
/// </summary>
struct Entity
{
	EntitySize id;
	EntitySize generation;
	bool enabled = true;
};
