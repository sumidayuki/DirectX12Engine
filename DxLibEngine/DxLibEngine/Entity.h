#pragma once

/// <summary>
/// �G���e�B�e�B��\���\���̂ł��B
/// ��ꐬ���FEntitySize(uint32_t)�^�� ID �G���e�B�e�B�ԍ���\���܂��B
/// ��񐬕��FEntitySize(uint32_t)�^�� generation ����ԍ���\���܂��B
/// </summary>
struct Entity
{
	EntitySize id;
	EntitySize generation;
	bool enabled = true;
};
