#pragma once
//------------------------------------------------------------------------------------------------------------
// このヘッダーファイル(Precompiled.h)は「コンパイル済みヘッダー」機能を使用する為のものです。
//------------------------------------------------------------------------------------------------------------

// DxLib
#include "DxLib.h"
#include <d3dcompiler.h>
#include <dwrite_3.h>
#include <DirectXMath.h>

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

//-----------------------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------------------

// Entityの型
using EntitySize = uint32_t;

//-----------------------------------------------------------------------------------------------------
// 前方宣言
//-----------------------------------------------------------------------------------------------------

// 数学
class Mathf;
class Vector2;
class Vector2Int;
class Vector3;
class Vector3Int;
class Vector4;
class Quaternion;
class Matrix4x4;
class Rect;


// 時間
class Time;

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

// システム関連
class Screen;

// 入力関連
class InputManager;
enum class KeyCode;
class ButtonControl;
class Keyboard;

//-----------------------------------------------------------------------------------------------------
// 自作ヘッダーファイル
//-----------------------------------------------------------------------------------------------------

// 数学
#include "Mathf.h"
#include "Vector2.h"
#include "Vector2Int.h"
#include "Vector3.h"
#include "Vector3Int.h"
#include "Vector4.h"
#include "Quaternion.h"
#include "Matrix4x4.h"
#include "Rect.h"

// 時間
#include "Time.h"

// システム関連
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

// Window関連
#include "Windows-NativeWindow.h"
#include "DxWindow.h"

// Scene
#include "Scene.h"
#include "SceneManager.h"

// 入力
#include "InputManager.h"
#include "KeyCode.h"
#include "ButtonControl.h"
#include "Keyboard.h"
