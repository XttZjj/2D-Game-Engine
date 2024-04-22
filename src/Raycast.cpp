#include "Raycast.h"

luabridge::LuaRef Raycast::PhsicsRaycast(b2Vec2 pos, b2Vec2 dir, float dist) {
    Raycast::HitResult hitResult;
    hitResult.actor = nullptr;
    hitResult.point.SetZero();
    hitResult.normal.SetZero();
    hitResult.is_trigger = false;
    dir.Normalize();
    b2RayCastInput input;
    input.p1 = pos;
    input.p2 = pos + dir.operator_mul(dist);
    input.maxFraction = 1.0f;

    RaycastCallback callback;

    // Perform the raycast
    Rigidbody::world->RayCast(&callback, pos, pos + dir.operator_mul(dist));

    // Retrieve hit information from the callback
    if (!callback.hitResults.empty()) {
        hitResult = callback.hitResults[0];
    }
    else {
        return luabridge::LuaRef(ComponentManager::lua_state);
    }
    return luabridge::LuaRef(ComponentManager::lua_state, hitResult);
}

float Raycast::RaycastCallback::ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) {
    Raycast::HitResult hitResult;
    hitResult.actor = reinterpret_cast<Actor*>(fixture->GetUserData().pointer);
    if (hitResult.actor == nullptr) {
        return -1.0f;
    }
    hitResult.point = point;
    hitResult.normal = normal;
    hitResult.is_trigger = fixture->IsSensor();
    hitResults.push_back(hitResult);

    return 0.0f; // Adjust ray length to continue raycast
}

luabridge::LuaRef Raycast::PhysicsRaycastAll(b2Vec2& pos, b2Vec2& dir, float dist) {
    // Normalize direction
    dir.Normalize();

    // Define ray cast input
    b2RayCastInput input;
    input.p1 = pos;
    input.p2 = pos + dir.operator_mul(dist);
    input.maxFraction = 1.0f;

    // Define ray cast callback
    class RaycastCallback : public b2RayCastCallback {
    public:
        std::vector<Raycast::HitResult> hitResults;

        float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) override {
            Raycast::HitResult hitResult;
            hitResult.actor = reinterpret_cast<Actor*>(fixture->GetUserData().pointer);
            if (hitResult.actor == nullptr) {
                return -1.0f; // Skip this fixture
            }
            hitResult.point = point;
            hitResult.normal = normal;
            hitResult.is_trigger = fixture->IsSensor();
            hitResults.push_back(hitResult);
            return 1.0f; // Continue ray cast
        }
    } callback;

    // Perform ray cast
    Rigidbody::world->RayCast(&callback, input.p1, input.p2);

    // Return hit results sorted by distance along the ray
    std::sort(callback.hitResults.begin(), callback.hitResults.end(), [&pos](const Raycast::HitResult& a, const Raycast::HitResult& b) {
        return (a.point - pos).Length() < (b.point - pos).Length();
        });

    luabridge::LuaRef componentTable = luabridge::newTable(ComponentManager::lua_state);
    int index = 1;
    for (auto pair : callback.hitResults) {
        componentTable[index++] = pair;
    }
    return componentTable;
}
