#pragma once

// シーンごとにシステムのインクルードを書くのがと、インクルードが大量になって読みにくくなるので、
// ここに System のインクルードをまとめています。
// ここに追加しないとシーンで System を追加できなかったりするので、System を追加した時は、忘れずに
// ここにインクルードを書く必要がある。

#include "CameraSystem.h"
#include "SpriteRendererSystem.h"
#include "MeshRendererSystem.h"
#include "CollisionSystem.h"
#include "TransformSystem.h"
#include "LightSystem.h"
#include "InputSystem.h"
#include "AnimationSystem.h"
#include "SkinnedMeshRendererSystem.h"