#include "Actor.h"
#include "lua.hpp"
#include "LuaBridge.h"
#include "SceneDB.h"
#include "TemplateDB.h"
#include <iostream>


// Constructor with renderer parameter
Actor::Actor()
    : name(""), view_image(""), view_image_back(""), view_image_damage(""), view_image_attack(""), damage_sfx(""), step_sfx(""), nearby_dialogue_sfx(""), transform_position_x(0), transform_position_y(0),
    transform_scale_x(1.0), transform_scale_y(1.0), transform_rotation_degrees(0),
    view_pivot_offset_x(0), view_pivot_offset_y(0), vel_x(0), vel_y(0),
    box_collider_width(0), box_collider_height(0), box_trigger_width(0), box_trigger_height(0), actor_id(0), relativeLocation(0), moving_up(false), movement_bounce_enabled(false), is_moving(false), dontDestroy(false) {}

// Destructor
Actor::~Actor() {
    // Free any allocated resources
    // For example: SDL_DestroyTexture(view_image);
}

// Copy constructor
Actor::Actor(const Actor& other) {
    // Copy all member variables from 'other'
    name = other.name;
    view_image = other.view_image;
    view_image_back = other.view_image_back;
    view_image_damage = other.view_image_damage;
    view_image_attack = other.view_image_attack;
    damage_sfx = other.damage_sfx;
    step_sfx = other.step_sfx;
    nearby_dialogue_sfx = other.nearby_dialogue_sfx;
    transform_position_x = other.transform_position_x;
    transform_position_y = other.transform_position_y;
    transform_scale_x = other.transform_scale_x;
    transform_scale_y = other.transform_scale_y;
    transform_rotation_degrees = other.transform_rotation_degrees;  
    view_pivot_offset_x = other.view_pivot_offset_x;
    view_pivot_offset_y = other.view_pivot_offset_y;
    vel_x = other.vel_x;
    vel_y = other.vel_y;
    box_collider_width = other.box_collider_width;
    box_collider_height = other.box_collider_height;
    box_trigger_width = other.box_trigger_width;
    box_trigger_height = other.box_trigger_height;
    nearby_dialogue = other.nearby_dialogue;
    contact_dialogue = other.contact_dialogue;
    actor_id = other.actor_id;
    relativeLocation = other.relativeLocation;
    render_order = other.render_order;
    movement_bounce_enabled = other.movement_bounce_enabled;
    dontDestroy = other.dontDestroy;
}

// Assignment operator
Actor& Actor::operator=(const Actor& other) {
    if (this != &other) {
        // Copy all member variables from 'other'
        name = other.name;
        view_image = other.view_image;
        view_image_back = other.view_image_back;
        view_image_damage = other.view_image_damage;
        view_image_attack = other.view_image_attack;
        damage_sfx = other.damage_sfx;
        step_sfx = other.step_sfx;
        nearby_dialogue_sfx = other.nearby_dialogue_sfx;
        transform_position_x = other.transform_position_x;
        transform_position_y = other.transform_position_y;
        transform_scale_x = other.transform_scale_x;
        transform_scale_y = other.transform_scale_y;
        transform_rotation_degrees = other.transform_rotation_degrees;
        view_pivot_offset_x = other.view_pivot_offset_x;
        view_pivot_offset_y = other.view_pivot_offset_y;
        vel_x = other.vel_x;
        vel_y = other.vel_y;
        box_collider_width = other.box_collider_width;
        box_collider_height = other.box_collider_height;
        box_trigger_width = other.box_trigger_width;
        box_trigger_height = other.box_trigger_height;
        nearby_dialogue = other.nearby_dialogue;
        contact_dialogue = other.contact_dialogue;
        actor_id = other.actor_id;
        relativeLocation = other.relativeLocation;
        render_order = other.render_order;
        movement_bounce_enabled = other.movement_bounce_enabled;
        dontDestroy = other.dontDestroy;
    }
    return *this;
}

bool Actor::actorComparator(const Actor* a, const Actor* b) {
    float renderOrderA = a->render_order.value_or(a->transform_position_y);
    float renderOrderB = b->render_order.value_or(b->transform_position_y);
    if (renderOrderA != renderOrderB) {
        return renderOrderA < renderOrderB;
    }
    else {
        return a->actor_id < b->actor_id;
    }
}

void Actor::RunOnStart() {
    for (auto it : components) {
        luabridge::LuaRef luaRef = *it.second;
        if (luaRef["OnStart"].isFunction() && luaRef["enabled"].cast<bool>() == true) {
            try {
                luaRef["OnStart"](luaRef);
            }
            catch (luabridge::LuaException e) {
                ReportError(e);
            }
        }
    }
}

bool isDigit(std::string c) {
    return c >= "0" && c <= "9";
}

void Actor::RunAddOnStart() {
    for (auto it : components) {
        luabridge::LuaRef luaRef = *it.second;
        if (luaRef["key"].cast<std::string>()[0] == 'r' && isDigit(luaRef["key"].cast<std::string>().substr(1)) && stoi(luaRef["key"].cast<std::string>().substr(1)) >= ComponentManager::nAddStart && stoi(luaRef["key"].cast<std::string>().substr(1)) <= ComponentManager::nAdd) {
            luaRef["enabled"] = true;
            if (luaRef["OnStart"].isFunction() && luaRef["enabled"].cast<bool>() == true) {
                try {
                    luaRef["OnStart"](luaRef);
                }
                catch (luabridge::LuaException e) {
                    ReportError(e);
                }
            }
            if (luaRef["Ready"].isFunction() && luaRef["Ready"].cast<bool>() == true) {
                try {
                    luaRef["Ready"](luaRef);
                }
                catch (luabridge::LuaException e) {
                    ReportError(e);
                }
            }
        }
    }
}

void Actor::RunOnUpdate() {
    for (auto it : components) {
        luabridge::LuaRef luaRef = *it.second;
        if (luaRef["OnUpdate"].isFunction() && luaRef["enabled"].cast<bool>() == true) {
            try {
                luaRef["OnUpdate"](luaRef);
            }
            catch (luabridge::LuaException e) {
                ReportError(e);
            }
        }
    }
}

void Actor::RunOnLateUpdate() {
    for (auto it : components) {
        luabridge::LuaRef luaRef = *it.second;
        if (luaRef["OnLateUpdate"].isFunction() && luaRef["enabled"].cast<bool>() == true) {
            try {
                luaRef["OnLateUpdate"](luaRef);
            }
            catch (luabridge::LuaException e) {
                ReportError(e);
            }
        }
    }
}

void Actor::RunOnDestroy() {
    for (auto it : components) {
        luabridge::LuaRef luaRef = *it.second;
        if (luaRef["OnDestroy"].isFunction()) {
            try {
                luaRef["OnDestroy"](luaRef);
            }
            catch (luabridge::LuaException e) {
                ReportError(e);
            }
        }
    }
}

void Actor::OnCollisionEnter(Rigidbody::Collision collision) {
    for (auto it : componentsWithCollideEnter) {
        luabridge::LuaRef luaRef = *it;
        if (luaRef["OnCollisionEnter"].isFunction() && luaRef["enabled"].cast<bool>() == true) {
            try {
                luaRef["OnCollisionEnter"](luaRef, collision);
            }
            catch (luabridge::LuaException e) {
                ReportError(e);
            }
        }
    }
}

void Actor::OnCollisionExit(Rigidbody::Collision collision) {
    for (auto it : componentsWithCollideExit) {
        luabridge::LuaRef luaRef = *it;
        if (luaRef["OnCollisionExit"].isFunction() && luaRef["enabled"].cast<bool>() == true) {
            try {
                luaRef["OnCollisionExit"](luaRef, collision);
            }
            catch (luabridge::LuaException e) {
                ReportError(e);
            }
        }
    }
}

void Actor::OnTriggerEnter(Rigidbody::Collision collision) {
    for (auto it : componentsWithTriggerEnter) {
        luabridge::LuaRef luaRef = *it;
        if (luaRef["OnTriggerEnter"].isFunction() && luaRef["enabled"].cast<bool>() == true) {
            try {
                luaRef["OnTriggerEnter"](luaRef, collision);
            }
            catch (luabridge::LuaException e) {
                ReportError(e);
            }
        }
    }
}

void Actor::OnTriggerExit(Rigidbody::Collision collision) {
    for (auto it : componentsWithTriggerExit) {
        luabridge::LuaRef luaRef = *it;
        if (luaRef["OnTriggerExit"].isFunction() && luaRef["enabled"].cast<bool>() == true) {
            try {
                luaRef["OnTriggerExit"](luaRef, collision);
            }
            catch (luabridge::LuaException e) {
                ReportError(e);
            }
        }
    }
}

void Actor::ReportError(const luabridge::LuaException& e) {
    std::string error_message = e.what();
    std::replace(error_message.begin(), error_message.end(), '\\', '/');
    std::cout << "\033[31m" << this->name << " : " << error_message << "\033[0m" << std::endl;
}

std::string Actor::GetName() {
    return this->name;
}

int Actor::GetID() {
    return this->actor_id;
}

void Actor::InjectConvenienceReferences(std::shared_ptr<luabridge::LuaRef> component_ref) {
    (*component_ref)["actor"] = this;
}

luabridge::LuaRef Actor::GetComponentByKey(std::string key) {
    for (auto it = components.begin(); it != components.end(); it++) {
        if (it->first == key) {
            return *it->second;
        }
    }
    return luabridge::LuaRef(ComponentManager::lua_state);
}

luabridge::LuaRef Actor::GetComponent(std::string type_name) {
    for (auto it = components.begin(); it != components.end(); it++) {
        luabridge::LuaRef component = *it->second;
        if (component["type"].cast<std::string>() == type_name) {
            return component;
        }
    }
    return luabridge::LuaRef(ComponentManager::lua_state);
}

luabridge::LuaRef Actor::GetComponents(std::string type_name) {
    luabridge::LuaRef componentTable = luabridge::newTable(ComponentManager::lua_state);
    int index = 1;
    for (auto pair : components) {
        luabridge::LuaRef component = *pair.second;
        if (component["type"].cast<std::string>() == type_name) {
            componentTable[index++] = component;
        }
    }
    return componentTable;
}


Actor* Actor::Find(std::string name) {
    for (int i = 0; i < SceneDB::actors.size(); i++) {
        if (SceneDB::actors[i]->name == name) {
            auto it = std::find(SceneDB::ActorsToRemove.begin(), SceneDB::ActorsToRemove.end(), SceneDB::actors[i]);
            if (it != SceneDB::ActorsToRemove.end()) {
                return luabridge::LuaRef(ComponentManager::lua_state);
            }
            return SceneDB::actors[i];
        }
    }
    for (int i = 0; i < SceneDB::ActorsToAdd.size(); i++) {
        if (SceneDB::ActorsToAdd[i]->name == name) {
            auto it = std::find(SceneDB::ActorsToRemove.begin(), SceneDB::ActorsToRemove.end(), SceneDB::actors[i]);
            if (it != SceneDB::ActorsToRemove.end()) {
                return luabridge::LuaRef(ComponentManager::lua_state);
            }
            return SceneDB::ActorsToAdd[i];
        }
    }
    return luabridge::LuaRef(ComponentManager::lua_state);
}

luabridge::LuaRef Actor::FindAll(std::string name) {
    luabridge::LuaRef componentTable = luabridge::newTable(ComponentManager::lua_state);
    int index = 1;
    for (int i = 0; i < SceneDB::actors.size(); i++) {
        if (SceneDB::actors[i]->name == name) {
            componentTable[index++] = SceneDB::actors[i];
        }
    }
    for (int i = 0; i < SceneDB::ActorsToAdd.size(); i++) {
        if (SceneDB::ActorsToAdd[i]->name == name) {
            componentTable[index++] = SceneDB::ActorsToAdd[i];
        }
    }
    return componentTable;
}

luabridge::LuaRef Actor::AddComponent(std::string type_name) {
    luabridge::LuaRef luaRef = luabridge::newTable(ComponentManager::lua_state);
    if (type_name != "Rigidbody") {
        ComponentManager::EstablishInheritance(luaRef, *ComponentManager::GetComponent(type_name));
    }
    else {
        luaRef = ComponentManager::CreateRigidbody();
    }
    std::string componentNmae = "r" + std::to_string(ComponentManager::nAdd);
    luaRef["key"] = componentNmae;
    ComponentManager::nAdd++;
    luaRef["type"] = type_name;
    luaRef["enabled"] = false;
    this->components[componentNmae] = std::make_shared<luabridge::LuaRef>(luaRef);
    this->InjectConvenienceReferences(this->components[componentNmae]);
    return luaRef;
}

void Actor::RemoveComponent(luabridge::LuaRef component_ref) {
    luabridge::LuaRef component = luabridge::newTable(ComponentManager::lua_state);
    std::string key = "";
    for (auto pair : components) {
        component = *pair.second;
        if (component == component_ref) {
            key = pair.first;
            break;
        }
    }
    component["enabled"] = false;
    if (component["OnDestroy"].isFunction()) {
        component["OnDestroy"](component);
    }
    components.erase(key);
}

Actor* Actor::Instantiate(std::string actor_template_name) {
    auto actor = new Actor;
    TemplateDB::applyTemplate(actor, actor_template_name);
    SceneDB::ActorsToAdd.push_back(actor);
    for (auto it = actor->components.begin(); it != actor->components.end(); it++) {
        actor->InjectConvenienceReferences(it->second);
    }
    return actor;
}

void Actor::Destroy(Actor* actor) {
    SceneDB::ActorsToRemove.push_back(actor);
    for (auto pair : actor->components) {
        luabridge::LuaRef component = *pair.second;
        component["enabled"] = false;
    }
}

void Actor::DontDestroy(Actor* actor) {
    actor->dontDestroy = true;
}