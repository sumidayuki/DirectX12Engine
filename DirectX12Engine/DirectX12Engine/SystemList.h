#pragma once

// シーンごとにシステムの登録を行うのが面倒なので、
// ここに全てのシステムをインクルードしておく

#include "CameraSystem.h"
#include "SpriteRendererSystem.h"
#include "MeshRendererSystem.h"
#include "CollisionSystem.h"
#include "TransformSystem.h"
#include "LightSystem.h"
#include "InputSystem.h"
#include "AnimationSystem.h"
#include "BoneSocketSystem.h"
#include "SkinnedMeshRendererSystem.h"
#include "GameManagerSystem.h"
#include "AIAgentSystem.h"
#include "PlayerSystem.h"
#include "PlayerCameraSystem.h"
#include "ProjectileSystem.h"
#include "EnemySystem.h"
#include "HPSystem.h"
#include "PhysicsSystem.h" // Added