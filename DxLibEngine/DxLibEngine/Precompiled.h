#pragma once
//------------------------------------------------------------------------------------------------------------
// このヘッダーファイル(Precompiled.h)は「コンパイル済みヘッダー」機能を使用する為のものです。
//------------------------------------------------------------------------------------------------------------

// Windows
#include <Unknwn.h>                         // COM(Component Object Mode)用インターフェイス
#include <windows.h>                        // Windows API
#include <wrl.h>                            // Windows実行時C++テンプレート
#include <shlwapi.h>                        // Windowsシェル

// C/C++
#include <clocale>                          // ロケール
#include <cstdio>                           // 標準入出力                        (stdio.h のC++版)
#include <cstdlib>                          // 標準ライブラリ                    (stdlib.h のC++版)
#include <cstdint>                          // 整数値に関する各種定義            (stdint.h のC++版)
#include <iostream>                         // 入出力ストリーム
#include <fstream>                          // ファイルストリーム
#include <sstream>                          // 文字列ストリーム
#include <cfloat>                           // 浮動小数点数に関するマクロ
#include <cmath>                            // 数学ライブラリ                    (math.h のC++版)
#include <climits>                          // 整数型に関するマクロ              (limits.h のC++版)
#include <thread>                           // スレッド
#include <cassert>                          // アサート機能                      (assert.h のC++版)

// C++標準テンプレートライブラリ (STL)
#include <string>                           // 文字列
#include <string_view>
#include <vector>                           // 可変長配列
#include <list>                             // 双方向リスト
#include <unordered_map>                    // 連想配列
#include <unordered_set>                    // 集合
#include <stack>                            // スタック
#include <queue>                            // キュー
#include <functional>                       // 関数オブジェクト
#include <chrono>                           // 時間
#include <algorithm>                        // アルゴリズム
#include <typeindex>						// タイプインデックス

// DirectX グラフィックスコンポーネント
#include <d3d12.h>                          // Direct3D12コアライブラリ
#include <d3dcompiler.h>                    // HLSLコンパイラ
#include <dxgi1_6.h>                        // DirectX Graphics Infrastructure
#include <DirectXMath.h>                    // DirectX数学ライブラリ
#include <dwrite_3.h>                       // DirectWrite
#include <d3d11on12.h>                      // Direct3D11 on Direct3D12
#include <d2d1_3.h>                         // Direct2D1

// DirectX オーディオコンポーネント
#include <xaudio2.h>                        // XAudio2

// DirectX 入力コンポーネント
#include <xinput.h>                         // XInput


//-----------------------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------------------

// COMポインタ
template<typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

// Entityの型
using EntitySize = uint32_t;

//------------------------------------------------------------------------------------------------------------
// コンパイル時定数
//------------------------------------------------------------------------------------------------------------

// Float::ToString()時の既定の書式文字列
static constexpr std::string_view DefaultFormatingOnFloatToString = "{:.2f}";

// VectorX::ToString()時の既定の書式文字列
static constexpr std::string_view DefaultFormatingOnVectorToString = "{:.2f}";

// VectorXInt::ToString()時の既定の書式文字列
static constexpr std::string_view DefaultFormatingOnVectorIntToString = "{:d}";

// Matrix4x4::ToString()時の既定の書式文字列
static constexpr std::string_view DefaultFormatingOnMatrixToString = "{:.5f}";

// Quaternion::ToString()時の既定の書式文字列
static constexpr std::string_view DefaultFormatingOnQuaternionToString = "{:.5f}";

// Color::ToString()時の既定の書式文字列
static constexpr std::string_view DefaultFormatingOnColorToString = "{:.3f}";

//-----------------------------------------------------------------------------------------------------
// 前方宣言
//-----------------------------------------------------------------------------------------------------

// 数学
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

// システム関連
class Reference;
class Screen;
class Time;

// シェーダー関連
class ShaderBytecode;

// アセット関連
class Texture;
class Texture2D;

// グラフィック関連
class FraneResource;
class GraohicsBuffer;
class Graphics;

//-------------------------------------------
// ECS関連
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

// World関連
class World;
template<typename First, typename... Rest> class View;

// Window関連
class DxWindow;

// Scene関連
class Scene;
class SceneManager;

// 入力関連
class InputManager;
enum class KeyCode;
class ButtonControl;
class Keyboard;

//-----------------------------------------------------------------------------------------------------
// 自作ヘッダーファイル
//-----------------------------------------------------------------------------------------------------

// 列挙型
#include "FilterMode.h"
#include "TextureWrapMode.h"
#include "TextureDimension.h"
 
// 数学
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

// 時間
#include "Time.h"

// システム関連
#include "Reference.h"
#include "Resolution.h"
#include "Screen.h"
#include "Windows-NativeWindow.h"
//#include "DxWindow.h"
#include "Application.h"

// グラフィックス関連
#include "DescriptorHeap.h"
#include "GraphicsBuffer.h"
#include "FrameResource.h"
#include "Graphics.h"

// シェーダー関連
#include "ShaderBytecode.h"

// アセット関連
#include "Texture.h"
#include "Texture2D.h"

// アセットインポーター関連
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

// 入力
#include "ButtonControl.h"
#include "KeyCode.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "InputManager.h"