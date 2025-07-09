#pragma once
//------------------------------------------------------------------------------------------------------------
// ���̃w�b�_�[�t�@�C��(Precompiled.h)�́u�R���p�C���ς݃w�b�_�[�v�@�\���g�p����ׂ̂��̂ł��B
//------------------------------------------------------------------------------------------------------------

// Windows
#include <Unknwn.h>                         // COM(Component Object Mode)�p�C���^�[�t�F�C�X
#include <windows.h>                        // Windows API
#include <wrl.h>                            // Windows���s��C++�e���v���[�g
#include <shlwapi.h>                        // Windows�V�F��

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
#include <string_view>
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

// DirectX �O���t�B�b�N�X�R���|�[�l���g
#include <d3d12.h>                          // Direct3D12�R�A���C�u����
#include <d3dcompiler.h>                    // HLSL�R���p�C��
#include <dxgi1_6.h>                        // DirectX Graphics Infrastructure
#include <DirectXMath.h>                    // DirectX���w���C�u����
#include <dwrite_3.h>                       // DirectWrite
#include <d3d11on12.h>                      // Direct3D11 on Direct3D12
#include <d2d1_3.h>                         // Direct2D1

// DirectX �I�[�f�B�I�R���|�[�l���g
#include <xaudio2.h>                        // XAudio2

// DirectX ���̓R���|�[�l���g
#include <xinput.h>                         // XInput


//-----------------------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------------------

// COM�|�C���^
template<typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

// Entity�̌^
using EntitySize = uint32_t;

//------------------------------------------------------------------------------------------------------------
// �R���p�C�����萔
//------------------------------------------------------------------------------------------------------------

// Float::ToString()���̊���̏���������
static constexpr std::string_view DefaultFormatingOnFloatToString = "{:.2f}";

// VectorX::ToString()���̊���̏���������
static constexpr std::string_view DefaultFormatingOnVectorToString = "{:.2f}";

// VectorXInt::ToString()���̊���̏���������
static constexpr std::string_view DefaultFormatingOnVectorIntToString = "{:d}";

// Matrix4x4::ToString()���̊���̏���������
static constexpr std::string_view DefaultFormatingOnMatrixToString = "{:.5f}";

// Quaternion::ToString()���̊���̏���������
static constexpr std::string_view DefaultFormatingOnQuaternionToString = "{:.5f}";

// Color::ToString()���̊���̏���������
static constexpr std::string_view DefaultFormatingOnColorToString = "{:.3f}";

//-----------------------------------------------------------------------------------------------------
// �O���錾
//-----------------------------------------------------------------------------------------------------

// ���w
class Mathf;
class Color;
class Plane;
class Bounds;
class Vector2;
class Vector2Int;
class Vector3;
class Vector3Int;
class Vector4;
class Quaternion;
class Matrix4x4;
class Rect;

// �V�X�e���֘A
class Reference;
class Screen;
class Time;

// �V�F�[�_�[�֘A
class ShaderBytecode;

// �A�Z�b�g�֘A
class Texture;
class Texture2D;

// �O���t�B�b�N�֘A
class FraneResource;
class GraohicsBuffer;
class Graphics;

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

// ���͊֘A
class InputManager;
enum class KeyCode;
class ButtonControl;
class Keyboard;

//-----------------------------------------------------------------------------------------------------
// ����w�b�_�[�t�@�C��
//-----------------------------------------------------------------------------------------------------

// �񋓌^
#include "FilterMode.h"
#include "TextureWrapMode.h"
#include "TextureDimension.h"
 
// ���w
#include "Mathf.h"
#include "Color.h"
#include "Vector2.h"
#include "Vector2Int.h"
#include "Vector3.h"
#include "Vector3Int.h"
#include "Vector4.h"
#include "Quaternion.h"
#include "Matrix4x4.h"
#include "Rect.h"
#include "Plane.h"
#include "Bounds.h"

// ����
#include "Time.h"

// �V�X�e���֘A
#include "Reference.h"
#include "Resolution.h"
#include "Screen.h"
#include "Windows-NativeWindow.h"
//#include "DxWindow.h"
#include "Application.h"

// �O���t�B�b�N�X�֘A
#include "DescriptorHeap.h"
#include "GraphicsBuffer.h"
#include "FrameResource.h"
#include "Graphics.h"

// �V�F�[�_�[�֘A
#include "ShaderBytecode.h"

// �A�Z�b�g�֘A
#include "Texture.h"
#include "Texture2D.h"

// �A�Z�b�g�C���|�[�^�[�֘A
#include "AssetImporter.h"
#include "TextureImporter.h"
#include "ModelImporter.h"

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
#include "SpriteRenderer.h"
#include "Mesh.h"
#include "Material.h"
#include "MeshRenderer.h"

#include "GeometryUtility.h"

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

// Scene
#include "Scene.h"
#include "SceneManager.h"

// ����
#include "ButtonControl.h"
#include "KeyCode.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "InputManager.h"