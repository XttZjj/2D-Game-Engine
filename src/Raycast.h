#ifndef RAYCAST_H
#define RAYCAST_H

#include <vector>
#include "Actor.h"
#include "Rigidbody.h"
#include "ComponentManager.h"
#include "box2d/box2d.h"

class Raycast {
public:
    class HitResult {
    public:
        Actor* actor;
        b2Vec2 point;
        b2Vec2 normal;
        bool is_trigger;
    };
    static luabridge::LuaRef PhsicsRaycast(b2Vec2 pos, b2Vec2 dir, float dist);

    static luabridge::LuaRef PhysicsRaycastAll(b2Vec2& pos, b2Vec2& dir, float dist);

    class RaycastCallback : public b2RayCastCallback {
    public:
        std::vector<Raycast::HitResult> hitResults;

        float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) override;
    };
};



#endif
