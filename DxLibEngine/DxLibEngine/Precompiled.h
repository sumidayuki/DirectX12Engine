#pragma once
//------------------------------------------------------------------------------------------------------------
// ���̃w�b�_�[�t�@�C��(Precompiled.h)�́u�R���p�C���ς݃w�b�_�[�v�@�\���g�p����ׂ̂��̂ł��B
//------------------------------------------------------------------------------------------------------------

// DxLib
#include "DxLib.h"
#include <d3dcompiler.h>
#include <dwrite_3.h>
#include <DirectXMath.h>

// C/C++
#include <clocale>                          // ���P�[��
#include <cstdio>                           // �W�����o��                        (stdio.h ��C++��)
#include <cstdlib>                          // �W�����C�u����                    (stdlib.h ��C++��)
#include <cstdint>                          // �����l�Ɋւ���e���`            (stdint.h ��C++��)
#include <iostream>                         // ���o�̓X�g���[��
#include <fstream>                          // �t�@�C���X�g���[��
#include <sstream>                          // ������X�g���[��
#include <cfloat>                           // ���������_���Ɋւ���}�N��
#include <cmath>                            // ���w���C�u����                    (math.h ��C++��)
#include <climits>                          // �����^�Ɋւ���}�N��              (limits.h ��C++��)
#include <thread>                           // �X���b�h
#include <cassert>                          // �A�T�[�g�@�\                      (assert.h ��C++��)

// C++�W���e���v���[�g���C�u���� (STL)
#include <string>                           // ������
#include <vector>                           // �ϒ��z��
#include <list>                             // �o�������X�g
#include <unordered_map>                    // �A�z�z��
#include <unordered_set>                    // �W��
#include <stack>                            // �X�^�b�N
#include <queue>                            // �L���[
#include <functional>                       // �֐��I�u�W�F�N�g
#include <chrono>                           // ����
#include <algorithm>                        // �A���S���Y��
#include <typeindex>						// �^�C�v�C���f�b�N�X

//-----------------------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------------------

// Entity�̌^
using EntitySize = uint32_t;

//-----------------------------------------------------------------------------------------------------
// �O���錾
//-----------------------------------------------------------------------------------------------------

// ���w
class Mathf;
class Vector2;
class Vector2Int;
class Vector3;
class Vector3Int;
class Vector4;
class Quaternion;
class Matrix4x4;
class Rect;


// ����
class Time;

//-------------------------------------------
// ECS�֘A
// ------------------------------------------
// Entity
struct Entity;
class EntityManager;			

// Component
class ComponentManager;
class IComponentStorage;
template<typename T> class ComponentStorage;

// System
class System;
class InputSystem;
class MovementSystem;
class SpriteRendererSystem;
class ShapeRendererSystem;
//-------------------------------------------

// World�֘A
class World;
template<typename First, typename... Rest> class View;

// Window�֘A
class DxWindow;

// Scene�֘A
class Scene;
class SceneManager;

// �V�X�e���֘A
class Screen;

// ���͊֘A
class InputManager;
enum class KeyCode;
class ButtonControl;
class Keyboard;

//-----------------------------------------------------------------------------------------------------
// ����w�b�_�[�t�@�C��
//-----------------------------------------------------------------------------------------------------

// ���w
#include "Mathf.h"
#include "Vector2.h"
#include "Vector2Int.h"
#include "Vector3.h"
#include "Vector3Int.h"
#include "Vector4.h"
#include "Quaternion.h"
#include "Matrix4x4.h"
#include "Rect.h"

// ����
#include "Time.h"

// �V�X�e���֘A
#include "Resolution.h"
#include "Screen.h"
#include "Application.h"

// Entity
#include "Entity.h"
#include "EntityManager.h"

// Utils
#include "Utils.h"

// Component
#include "ComponentManager.h"
#include "IComponentStorage.h"
#include "ComponentStorage.h"
#include "Input.h"
#include "Sprite.h"
#include "Velocity.h"
#include "Circle.h"
#include "Triangle.h"
#include "Box.h"
#include "Target.h"
#include "Transform.h"
#include "Camera.h"
#include "RenderCommand.h"

#include "Player.h";
#include "Bullet.h";
#include "EnemyFormation.h"
#include "EnemySpawnInfo.h"
#include "EnemySpawnMap.h"
#include "Enemy.h"
#include "CollisionInfo.h"
#include "CircleCollider2D.h"
#include "BoxCollider2D.h"
#include "status.h"
#include "Boss.h"
#include "BossBullet.h"

// System
#include "System.h"
/*
#include "InputSystem.h"
#include "MovementSystem.h"
#include "SpriteRendererSystem.h"
#include "ShapeRendererSystem.h"
*/

// World
#include "World.h"
#include "View.h"

// Window�֘A
#include "Windows-NativeWindow.h"
#include "DxWindow.h"

// Scene
#include "Scene.h"
#include "SceneManager.h"

// ����
#include "InputManager.h"
#include "KeyCode.h"
#include "ButtonControl.h"
#include "Keyboard.h"
