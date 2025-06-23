#pragma once

// シーンごとにシステムのインクルードを書くのがと、インクルードが大量になって読みにくくなるので、
// ここに System のインクルードをまとめています。
// ここに追加しないとシーンで System を追加できなかったりするので、System を追加した時は、忘れずに
// ここにインクルードを書く必要がある。

#include "CameraSystem.h"
#include "InputSystem.h"
#include "MovementSystem.h"
#include "RendererSystem.h"
#include "PlayerSystem.h"
#include "BulletSystem.h"
#include "EnemySpawnerSystem.h"
#include "EnemyStandardSystem.h"
#include "EnemyVSystem.h"
#include "EnemyCircleSystem.h"
#include "EnemyWaveSystem.h"
#include "EnemySystem.h"
#include "BossSystem.h"
#include "BossBulletSystem.h"
#include "CollisionSystem.h"
#include "TransformSystem.h"