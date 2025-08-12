#pragma once

// C++��HLSL�ō\���̂����L���邽�߂̏���
#ifdef __cplusplus
using float3 = Vector3;
using float4 = Color;
#endif


enum class LightType
{
	Directional = 0, 	// ����������ψ�Ɍ��𓖂Ă�
	Point = 1,			// �_���狅��Ɍ��𓖂Ă�
	Spot = 2,			// �~����Ɍ��𓖂Ă�
};

struct Light
{
	float4 color = { 1.0f, 1.0f, 1.0f, 1.0f };	// ���C�g�̐F�Ƌ��x
	float3 position = { 0.0f, 0.0f, 0.0f };		// Point, Spot�Ŏg�p
	float  range = 10.0f;						// Point, Spot�Ŏg�p�i���̓͂��ő勗���j
	float3 direction = { 0.0f, -1.0f, 0.0f };	// Directional, Spot�Ŏg�p
	float  spotAngle = 30.0f;					// Spot�Ŏg�p�i�~���̊p�x�j

	LightType type = LightType::Directional;
	int enabled = 1;							// �L���t���O (bool��HLSL�Ƃ̌݊����Ŗ�肪�N���₷������int)

	// HLSL��StructuredBuffer�̗v�f��16�o�C�g�A���C�����g����邽�߁A
	// C++���ł��T�C�Y�����킹��
	float padding[2];

};