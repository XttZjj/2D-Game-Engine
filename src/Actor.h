#ifndef ACTOR_H
#define ACTOR_H

//#include <SDL.h>
#include <string>
#include <optional>
#include <unordered_set>
#include <memory>
#include "lua.hpp"
#include "LuaBridge.h"
#include "Rigidbody.h"

class Actor {
public:
    // Constructor with renderer parameter
    Actor();

    // Default destructor
    ~Actor();

    // Copy constructor
    Actor(const Actor& other);

    // Assignment operator
    Actor& operator=(const Actor& other);

    static bool actorComparator(const Actor* a, const Actor* b);

    std::string GetName();

    int GetID();

    luabridge::LuaRef GetComponentByKey(std::string key);

    luabridge::LuaRef GetComponent(std::string type_name);

    luabridge::LuaRef GetComponents(std::string type_name);

    luabridge::LuaRef AddComponent(std::string type_name);

    void RemoveComponent(luabridge::LuaRef component_ref);

    void InjectConvenienceReferences(std::shared_ptr<luabridge::LuaRef> component_ref);

    void RunOnStart();

    void RunAddOnStart();

    void RunOnUpdate();

    void RunOnLateUpdate();

    void RunOnDestroy();

    void ReportError(const luabridge::LuaException& e);

    std::string name;
    std::string view_image;
    std::string view_image_back;
    std::string view_image_damage;
    std::string view_image_attack;
    std::string damage_sfx;
    std::string step_sfx;
    std::string nearby_dialogue_sfx;
    double transform_position_x;
    double transform_position_y;
    double transform_scale_x;
    double transform_scale_y;
    double transform_rotation_degrees;
    double view_pivot_offset_x;
    double view_pivot_offset_y;
    double vel_x;
    double vel_y;
    float box_collider_width;
    float box_collider_height;
    float box_trigger_width;
    float box_trigger_height;
    std::string nearby_dialogue;
    std::string contact_dialogue;
    int actor_id;
    int relativeLocation;
    std::optional<double> render_order;
    bool moving_up;
    bool movement_bounce_enabled;
    bool is_moving;
    bool dontDestroy;

    std::unordered_set<Actor*> colliding_actors_this_frame;
    std::unordered_set<Actor*> triggered_actors_this_frame;
    std::map<std::string, std::shared_ptr<luabridge::LuaRef>> components;
    std::vector<std::shared_ptr<luabridge::LuaRef>> componentsWithCollideEnter;
    std::vector<std::shared_ptr<luabridge::LuaRef>> componentsWithCollideExit;
    std::vector<std::shared_ptr<luabridge::LuaRef>> componentsWithTriggerEnter;
    std::vector<std::shared_ptr<luabridge::LuaRef>> componentsWithTriggerExit;

    void OnCollisionEnter(Rigidbody::Collision collision);
    void OnCollisionExit(Rigidbody::Collision collision);
    void OnTriggerEnter(Rigidbody::Collision collision);
    void OnTriggerExit(Rigidbody::Collision collision);

    static Actor* Find(std::string name);

    static luabridge::LuaRef FindAll(std::string name);

    static Actor* Instantiate(std::string actor_template_name);

    static void Destroy(Actor* actor);

    static void DontDestroy(Actor* actor);
};

#endif // ACTOR_H
