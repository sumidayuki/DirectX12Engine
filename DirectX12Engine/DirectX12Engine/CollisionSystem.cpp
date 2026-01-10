#include "Precompiled.h"
#include "CollisionSystem.h"
#include "Collider.h"
#include "Rigidbody.h"
#include "Transform.h"
#include "Entity.h"
#include "World.h"
#include "Mathf.h"
#include "Bounds.h"
#include "DebugManager.h"
#include <vector>
#include <cmath>

void CollisionSystem::Update(World& world)
{
    View<Collider, Transform> colliderView(world);
    
    std::vector<Entity> activeEntities;
    std::vector<Entity> passiveEntities;

    for (auto [entity, col, transform] : colliderView)
    {
        if (!col.isEnable) continue;
        
        if (col.info.state == CollisionState::Enter || col.info.state == CollisionState::Stay)
            col.info.state = CollisionState::Exit;
        else if (col.info.state == CollisionState::Exit)
            col.info.state = CollisionState::None;
        
        Rigidbody* rb = world.GetComponent<Rigidbody>(entity);
        if (rb || col.isTrigger)
        {
            activeEntities.push_back(entity);
        }
        passiveEntities.push_back(entity);
    }

    for (Entity activeEntity : activeEntities)
    {
        Collider* myCol = world.GetComponent<Collider>(activeEntity);
        Transform* myTrans = world.GetComponent<Transform>(activeEntity);
        Rigidbody* myRb = world.GetComponent<Rigidbody>(activeEntity);
        
        if (!myCol || !myTrans) continue;

        Vector3 myPos = myTrans->position + myCol->offset;
        
        Bounds myBounds;
        if (myCol->type == ColliderType::Box || myCol->type == ColliderType::AABB)
        {
            myBounds = Bounds(myPos, myCol->size * myTrans->scale);
        }

        for (Entity passiveEntity : passiveEntities)
        {
            if (activeEntity == passiveEntity) continue;

            Collider* otherCol = world.GetComponent<Collider>(passiveEntity);
            Transform* otherTrans = world.GetComponent<Transform>(passiveEntity);
            
            if (!otherCol || !otherTrans) continue;

            Vector3 otherPos = otherTrans->position + otherCol->offset;
            bool isColliding = false;
            Vector3 penetration = Vector3::zero;

            if (myCol->type == ColliderType::Sphere && otherCol->type == ColliderType::Sphere)
            {
                // Sphere vs Sphere
                float distSq = (myPos - otherPos).SqrMagnitude();
                float r1 = myCol->radius * myTrans->scale.x;
                float r2 = otherCol->radius * otherTrans->scale.x;
                float radSum = r1 + r2;
                
                if (distSq < radSum * radSum)
                {
                    isColliding = true;
                    float dist = sqrt(distSq);
                    if (dist < 0.0001f) penetration = Vector3::up * 0.01f;
                    else penetration = ((myPos - otherPos) / dist) * (radSum - dist);
                }
            }
            else if ((myCol->type == ColliderType::Box || myCol->type == ColliderType::AABB) && 
                     (otherCol->type == ColliderType::Box || otherCol->type == ColliderType::AABB))
            {
                // Box vs Box (AABB)
                Bounds otherBounds(otherPos, otherCol->size * otherTrans->scale);
                
                if (myBounds.Intersects(otherBounds))
                {
                    isColliding = true;
                    Vector3 minA = myBounds.GetMin();    Vector3 maxA = myBounds.GetMax();
                    Vector3 minB = otherBounds.GetMin(); Vector3 maxB = otherBounds.GetMax();

                    float overlapX = std::min(maxA.x, maxB.x) - std::max(minA.x, minB.x);
                    float overlapY = std::min(maxA.y, maxB.y) - std::max(minA.y, minB.y);
                    float overlapZ = std::min(maxA.z, maxB.z) - std::max(minA.z, minB.z);

                    Vector3 centerA = myBounds.GetCenter();
                    Vector3 centerB = otherBounds.GetCenter();

                    if (overlapX < overlapY && overlapX < overlapZ)
                        penetration.x = (centerA.x > centerB.x) ? overlapX : -overlapX;
                    else if (overlapY < overlapZ)
                        penetration.y = (centerA.y > centerB.y) ? overlapY : -overlapY;
                    else
                        penetration.z = (centerA.z > centerB.z) ? overlapZ : -overlapZ;
                }
            }
            else
            {
                // Sphere vs Box
                Vector3 sCenter;
                float sRadius;
                Bounds bBounds;
                bool swap = false;

                if (myCol->type == ColliderType::Sphere)
                {
                    sCenter = myPos;
                    sRadius = myCol->radius * myTrans->scale.x;
                    bBounds = Bounds(otherPos, otherCol->size * otherTrans->scale);
                }
                else
                {
                    sCenter = otherPos;
                    sRadius = otherCol->radius * otherTrans->scale.x;
                    bBounds = myBounds;
                    swap = true;
                }

                if (bBounds.SqrDistance(sCenter) < sRadius * sRadius)
                {
                    isColliding = true;
                    Vector3 closest = bBounds.ClosestPoint(sCenter);
                    Vector3 dir = sCenter - closest;
                    float dist = dir.Magnitude();

                    Vector3 pen;
                    if (dist < 0.0001f)
                    {
                         // Sphere center inside box
                         if (bBounds.SqrDistance(sCenter) < 0.00001f) pen = Vector3::up * sRadius;
                         else pen = dir.Normalized() * sRadius; 
                    }
                    else
                    {
                        pen = (dir / dist) * (sRadius - dist);
                    }
                    
                    if (swap) penetration = -pen;
                    else penetration = pen;
                }
            }

            if (isColliding)
            {
                auto UpdateState = [](CollisionInfo& info) 
                {
                     if (info.state == CollisionState::None) info.state = CollisionState::Enter;
                     else info.state = CollisionState::Stay;
                };
                
                UpdateState(myCol->info);
                myCol->info.other = passiveEntity;

                UpdateState(otherCol->info); 
                otherCol->info.other = activeEntity;

                if (myRb && !myCol->isTrigger && !otherCol->isTrigger)
                {
                    if (std::isfinite(penetration.x) && std::isfinite(penetration.y) && std::isfinite(penetration.z))
                    {
                        myTrans->position += penetration;
                        myTrans->dirty = true;
                        
                        Vector3 normal = penetration.Normalized();
                         if (std::isfinite(normal.x) && std::isfinite(normal.y) && std::isfinite(normal.z))
                         {
                             float vel = Vector3::Dot(myRb->velocity, normal);
                             if (vel < 0)
                             {
                                 myRb->velocity -= normal * (vel * (1.0f + 0.5f)); // Bounce
                             }
                         }
                    }
                }
            }
        }
    }

    for (auto [entity, col, transform] : colliderView)
    {
        Vector3 center = transform.position + col.offset;
        Color color = Color::yellow;
        if (col.isEnable)
        {
            color = (col.info.state == CollisionState::Enter || col.info.state == CollisionState::Stay) ? Color::red : Color::green;
        }

        if (col.type == ColliderType::Sphere)
        {
            DebugManager::GetInstance()->DrawSphere(center, col.radius * transform.scale.x, color);
        }
        else
        {
             // Box/AABB
             DebugManager::GetInstance()->DrawAABB(center - (col.size * 0.5f), center + (col.size * 0.5f), color);
        }
    }
}