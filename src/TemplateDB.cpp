#include "TemplateDB.h"
#include "SceneDB.h"
#pragma warning(disable: 4996)
#include "../Third Party/rapidjson-1.1.0/include/rapidjson/document.h"
#include "../Third Party/rapidjson-1.1.0/include/rapidjson/filereadstream.h"
#include <filesystem>
#include <iostream>

using namespace rapidjson;
using namespace std;
namespace fs = std::filesystem;

TemplateDB::TemplateDB() {}

Actor* TemplateDB::loadTemplates(const std::string& tempName) {
    if (!filesystem::exists("resources/actor_templates/" + tempName +".template")) {
        cout << "error: template " << tempName << " is missing";
        exit(0);
    }
    rapidjson::Document a;
    string path = "resources/actor_templates/" + tempName + ".template";
    FILE* file_pointer = nullptr;
#ifdef _WIN32
    fopen_s(&file_pointer, path.c_str(), "rb");
#else
    file_pointer = fopen(path.c_str(), "rb");
#endif
    char buffer[65536];
    rapidjson::FileReadStream stream(file_pointer, buffer, sizeof(buffer));
    a.ParseStream(stream);
    std::fclose(file_pointer);

    if (a.HasParseError()) {
        rapidjson::ParseErrorCode errorCode = a.GetParseError();
        std::cout << "error parsing json at [" << path << "]" << std::endl;
        exit(0);
    }
    auto actor = new Actor;
    // Populate actor from the JSON document
    if (a.HasMember("name") && a["name"].IsString()) {
        actor->name = a["name"].GetString();
    }
    if (a.HasMember("components")) {
        for (Value::ConstMemberIterator itr = a["components"].MemberBegin(); itr != a["components"].MemberEnd(); ++itr) {
            const std::string componentName = itr->name.GetString();
            const std::string componentType = itr->value["type"].GetString();
            luabridge::LuaRef luaRef = luabridge::newTable(ComponentManager::lua_state);
            if (componentType == "Rigidbody") {
                luaRef = ComponentManager::CreateRigidbody();
            }
            else {
                ComponentManager::EstablishInheritance(luaRef, *ComponentManager::GetComponent(componentType));
            }
            luaRef["key"] = componentName;
            luaRef["type"] = componentType;
            luaRef["enabled"] = true;
            for (Value::ConstMemberIterator propItr = itr->value.MemberBegin(); propItr != itr->value.MemberEnd(); ++propItr) {
                const auto& propName = propItr->name.GetString();
                const auto& propValue = propItr->value;
                if (propItr->name.IsString() && std::strcmp(propItr->name.GetString(), "type") == 0) {
                    continue;
                }
                if (propValue.IsDouble()) {
                    luaRef[propName] = propValue.GetDouble();
                }
                else if (propValue.IsInt()) {
                    luaRef[propName] = propValue.GetInt();
                }
                else if (propValue.IsBool()) {
                    luaRef[propName] = propValue.GetBool();
                }
                else {
                    luaRef[propName] = propValue.GetString();
                }
            }
            std::cout << luaRef["type"].cast<std::string>() << 2222 << std::endl;
            actor->components[componentName] = std::make_shared<luabridge::LuaRef>(luaRef);
        }
    }
    if (a.HasMember("view_image") && a["view_image"].IsString()) {
        actor->view_image = a["view_image"].GetString();
    }
    if (a.HasMember("view_image_back") && a["view_image_back"].IsString()) {
        actor->view_image_back = a["view_image_back"].GetString();
    }
    if (a.HasMember("view_image_damage") && a["view_image_damage"].IsString()) {
        actor->view_image_damage = a["view_image_damage"].GetString();
    }
    if (a.HasMember("view_image_attack") && a["view_image_attack"].IsString()) {
        actor->view_image_attack = a["view_image_attack"].GetString();
    }
    if (a.HasMember("damage_sfx") && a["damage_sfx"].IsString()) {
        actor->damage_sfx = a["damage_sfx"].GetString();
    }
    if (a.HasMember("step_sfx") && a["step_sfx"].IsString()) {
        actor->step_sfx = a["step_sfx"].GetString();
    }
    if (a.HasMember("nearby_dialogue_sfx") && a["nearby_dialogue_sfx"].IsString()) {
        actor->nearby_dialogue_sfx = a["nearby_dialogue_sfx"].GetString();
    }
    if (a.HasMember("transform_position_x") && a["transform_position_x"].IsNumber()) {
        if (a["transform_position_x"].IsInt()) {
            actor->transform_position_x = a["transform_position_x"].GetInt();
        }
        else {
            actor->transform_position_x = a["transform_position_x"].GetDouble();
        }
    }
    if (a.HasMember("transform_position_y") && a["transform_position_y"].IsNumber()) {
        if (a["transform_position_y"].IsInt()) {
            actor->transform_position_y = a["transform_position_y"].GetInt();
        }
        else {
            actor->transform_position_y = a["transform_position_y"].GetDouble();
        }
    }
    if (a.HasMember("transform_scale_x") && a["transform_scale_x"].IsNumber()) {
        if (a["transform_scale_x"].IsInt()) {
            actor->transform_scale_x = a["transform_scale_x"].GetInt();
        }
        else {
            actor->transform_scale_x = a["transform_scale_x"].GetDouble();
        }
    }
    if (a.HasMember("transform_scale_y") && a["transform_scale_y"].IsNumber()) {
        if (a["transform_scale_y"].IsInt()) {
            actor->transform_scale_y = a["transform_scale_y"].GetInt();
        }
        else {
            actor->transform_scale_y = a["transform_scale_y"].GetDouble();
        }
    }
    if (a.HasMember("transform_rotation_degrees") && a["transform_rotation_degrees"].IsNumber()) {
        if (a["transform_rotation_degrees"].IsInt()) {
            actor->transform_rotation_degrees = a["transform_rotation_degrees"].GetInt();
        }
        else {
            actor->transform_rotation_degrees = a["transform_rotation_degrees"].GetDouble();
        }
    }
    if (a.HasMember("view_pivot_offset_x") && a["view_pivot_offset_x"].IsNumber()) {
        if (a["view_pivot_offset_x"].IsInt()) {
            actor->view_pivot_offset_x = a["view_pivot_offset_x"].GetInt();
        }
        else {
            actor->view_pivot_offset_x = a["view_pivot_offset_x"].GetDouble();
        }
    }
    if (a.HasMember("view_pivot_offset_y") && a["view_pivot_offset_y"].IsNumber()) {
        if (a["view_pivot_offset_y"].IsInt()) {
            actor->view_pivot_offset_y = a["view_pivot_offset_y"].GetInt();
        }
        else {
            actor->view_pivot_offset_y = a["view_pivot_offset_y"].GetDouble();
        }
    }
    if (a.HasMember("vel_x") && a["vel_x"].IsNumber()) {
        if (a["vel_x"].IsInt()) {
            actor->vel_x = a["vel_x"].GetInt();
        }
        else {
            actor->vel_x = a["vel_x"].GetDouble();
        }
    }
    if (a.HasMember("vel_y") && a["vel_y"].IsNumber()) {
        if (a["vel_y"].IsInt()) {
            actor->vel_y = a["vel_y"].GetInt();
        }
        else {
            actor->vel_y = a["vel_y"].GetDouble();
        }
    }
    if (a.HasMember("box_collider_width") && a["box_collider_width"].IsNumber()) {
        if (a["box_collider_width"].IsInt()) {
            actor->box_collider_width = a["box_collider_width"].GetInt() * std::abs(actor->transform_scale_x);
        }
        else {
            actor->box_collider_width = a["box_collider_width"].GetDouble() * std::abs(actor->transform_scale_x);
        }
    }
    if (a.HasMember("box_collider_height") && a["box_collider_height"].IsNumber()) {
        if (a["box_collider_height"].IsInt()) {
            actor->box_collider_height = a["box_collider_height"].GetInt() * std::abs(actor->transform_scale_y);
        }
        else {
            actor->box_collider_height = a["box_collider_height"].GetDouble() * std::abs(actor->transform_scale_y);
        }
    }
    if (a.HasMember("box_trigger_width") && a["box_trigger_width"].IsNumber()) {
        if (a["box_trigger_width"].IsInt()) {
            actor->box_trigger_width = a["box_trigger_width"].GetInt() * std::abs(actor->transform_scale_x);
        }
        else {
            actor->box_trigger_width = a["box_trigger_width"].GetDouble() * std::abs(actor->transform_scale_x);
        }
    }
    if (a.HasMember("box_trigger_height") && a["box_trigger_height"].IsNumber()) {
        if (a["box_trigger_height"].IsInt()) {
            actor->box_trigger_height = a["box_trigger_height"].GetInt() * std::abs(actor->transform_scale_y);
        }
        else {
            actor->box_trigger_height = a["box_trigger_height"].GetDouble() * std::abs(actor->transform_scale_y);
        }
    }
    if (a.HasMember("movement_bounce_enabled") && a["movement_bounce_enabled"].IsBool()) {
        actor->movement_bounce_enabled = a["movement_bounce_enabled"].GetBool();
    }
    if (a.HasMember("nearby_dialogue") && a["nearby_dialogue"].IsString()) {
        actor->nearby_dialogue = a["nearby_dialogue"].GetString();
    }
    if (a.HasMember("contact_dialogue") && a["contact_dialogue"].IsString()) {
        actor->contact_dialogue = a["contact_dialogue"].GetString();
    }
    if (a.HasMember("render_order") && a["render_order"].IsInt()) {
        actor->render_order = a["render_order"].GetInt();
    }
    templates[tempName] = actor;
    return actor;
}

void TemplateDB::applyTemplate(Actor*& actor, const std::string& templateName) {
    auto it = templates.find(templateName);
    if (it != templates.end()) {
        const Actor* templateActor = it->second;
        // Apply templateActor properties to actor using assignment operator
        *actor = Actor(*templateActor);
        for (auto it : templateActor->components) {
            luabridge::LuaRef luaRef = luabridge::newTable(ComponentManager::lua_state);
            luabridge::LuaRef component = *it.second;
            if (component["type"].cast<std::string>() != "Rigidbody") {
                ComponentManager::EstablishInheritance(luaRef, *it.second);
            }
            else {
                luaRef = ComponentManager::CreateRigidbody();
                luaRef["key"] = component["key"];
                luaRef["enabled"] = component["enabled"];
                luaRef["actor"] = component["actor"];
            }
            actor->components[it.first] = std::make_shared<luabridge::LuaRef>(luaRef);
        }
        for (auto it : templateActor->componentsWithCollideEnter) {
            luabridge::LuaRef luaRef = luabridge::newTable(ComponentManager::lua_state);
            ComponentManager::EstablishInheritance(luaRef, *it);
            actor->componentsWithCollideEnter.push_back(std::make_shared<luabridge::LuaRef>(luaRef));
        }
        for (auto it : templateActor->componentsWithCollideExit) {
            luabridge::LuaRef luaRef = luabridge::newTable(ComponentManager::lua_state);
            ComponentManager::EstablishInheritance(luaRef, *it);
            actor->componentsWithCollideExit.push_back(std::make_shared<luabridge::LuaRef>(luaRef));
        }
        for (auto it : templateActor->componentsWithTriggerEnter) {
            luabridge::LuaRef luaRef = luabridge::newTable(ComponentManager::lua_state);
            ComponentManager::EstablishInheritance(luaRef, *it);
            actor->componentsWithTriggerEnter.push_back(std::make_shared<luabridge::LuaRef>(luaRef));
        }
        for (auto it : templateActor->componentsWithTriggerExit) {
            luabridge::LuaRef luaRef = luabridge::newTable(ComponentManager::lua_state);
            ComponentManager::EstablishInheritance(luaRef, *it);
            actor->componentsWithTriggerExit.push_back(std::make_shared<luabridge::LuaRef>(luaRef));
        }
    }
    else {
        *actor = Actor(*loadTemplates(templateName));
        const Actor* templateActor = loadTemplates(templateName);
        for (auto it : templateActor->components) {
            luabridge::LuaRef luaRef = luabridge::newTable(ComponentManager::lua_state);
            luabridge::LuaRef component = *it.second;
            if (component["type"].cast<std::string>() != "Rigidbody") {
                std::cout << component["type"].cast<std::string>() << std::endl;
                ComponentManager::EstablishInheritance(luaRef, *it.second);
            }
            else {
                luaRef = ComponentManager::CreateRigidbody();
                luaRef["key"] = component["key"];
                luaRef["enabled"] = component["enabled"];
                luaRef["actor"] = component["actor"];
            }
            std::cout << luaRef["type"].cast<std::string>() << std::endl;
            actor->components[it.first] = std::make_shared<luabridge::LuaRef>(luaRef);
        }
        for (auto it : templateActor->componentsWithCollideEnter) {
            luabridge::LuaRef luaRef = luabridge::newTable(ComponentManager::lua_state);
            ComponentManager::EstablishInheritance(luaRef, *it);
            actor->componentsWithCollideEnter.push_back(std::make_shared<luabridge::LuaRef>(luaRef));
        }
        for (auto it : templateActor->componentsWithCollideExit) {
            luabridge::LuaRef luaRef = luabridge::newTable(ComponentManager::lua_state);
            ComponentManager::EstablishInheritance(luaRef, *it);
            actor->componentsWithCollideExit.push_back(std::make_shared<luabridge::LuaRef>(luaRef));
        }
        for (auto it : templateActor->componentsWithTriggerEnter) {
            luabridge::LuaRef luaRef = luabridge::newTable(ComponentManager::lua_state);
            ComponentManager::EstablishInheritance(luaRef, *it);
            actor->componentsWithTriggerEnter.push_back(std::make_shared<luabridge::LuaRef>(luaRef));
        }
        for (auto it : templateActor->componentsWithTriggerExit) {
            luabridge::LuaRef luaRef = luabridge::newTable(ComponentManager::lua_state);
            ComponentManager::EstablishInheritance(luaRef, *it);
            actor->componentsWithTriggerExit.push_back(std::make_shared<luabridge::LuaRef>(luaRef));
        }
    }
}
