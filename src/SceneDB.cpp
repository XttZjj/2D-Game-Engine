#include "SceneDB.h"
#pragma warning(disable: 4996)
#include "../Third Party/rapidjson-1.1.0/include/rapidjson/document.h"
#include "../Third Party/rapidjson-1.1.0/include/rapidjson/filereadstream.h"
#include <filesystem>
#include <iostream>
#include "TemplateDB.h"
#include "../Third Party/glm/glm/glm.hpp"
#include "../SDL2/SDL.h"
#include <sstream>
#include <algorithm>
#include <limits>
#include <unordered_map>

using namespace rapidjson;
using namespace std;
namespace fs = std::filesystem;

SceneDB::SceneDB() {}

void SceneDB::loadScene(const std::string& sceneName) {
    if (!filesystem::exists("resources/scenes/" + sceneName + ".scene")) {
        cout << "error: scene " << sceneName << " is missing";
        exit(0);
    }
    rapidjson::Document doc;
    string path = "resources/scenes/" + sceneName + ".scene";
    FILE* file_pointer = nullptr;
#ifdef _WIN32
    fopen_s(&file_pointer, path.c_str(), "rb");
#else
    file_pointer = fopen(path.c_str(), "rb");
#endif
    char buffer[65536];
    rapidjson::FileReadStream stream(file_pointer, buffer, sizeof(buffer));
    doc.ParseStream(stream);
    std::fclose(file_pointer);
    TemplateDB templateDB;
    if (doc.HasParseError()) {
        rapidjson::ParseErrorCode errorCode = doc.GetParseError();
        cout << "error parsing json at [" << path << "]" << std::endl;
        exit(0);
    }
    if (doc.HasMember("actors") && doc["actors"].IsArray()) {
        const rapidjson::Value& actorsArray = doc["actors"];
        const size_t numActors = actorsArray.Size();
        actors.reserve(numActors);
        int id = 0;
        for (size_t i = 0; i < numActors; ++i) {
            const auto& a = actorsArray[i];
            auto actor = new Actor;
            const auto templateMember = a.FindMember("template");
            if (templateMember != a.MemberEnd() && templateMember->value.IsString()) {
                templateDB.applyTemplate(actor, templateMember->value.GetString());
            }
            if (a.HasMember("name") && a["name"].IsString()) {
                actor->name = a["name"].GetString();
            }
            if (a.HasMember("components")) {
                for (Value::ConstMemberIterator itr = a["components"].MemberBegin(); itr != a["components"].MemberEnd(); ++itr) {
                    const std::string componentName = itr->name.GetString();
                    luabridge::LuaRef luaRef = luabridge::newTable(ComponentManager::lua_state);
                    if (actor->components.find(componentName) == actor->components.end()) {
                        const std::string componentType = itr->value["type"].GetString();
                        if (componentType == "Rigidbody") {
                            luaRef = ComponentManager::CreateRigidbody();
                        }
                        else {
                            ComponentManager::EstablishInheritance(luaRef, *ComponentManager::GetComponent(componentType));
                        }
                        luaRef["key"] = componentName;
                        luaRef["type"] = componentType;
                        luaRef["enabled"] = true;
                    }
                    else {
                        luaRef = *actor->components[componentName];
                    }
                    for (Value::ConstMemberIterator propItr = itr->value.MemberBegin(); propItr != itr->value.MemberEnd(); ++propItr) {
                        const auto& propName = propItr->name.GetString();
                        const auto& propValue = propItr->value;
                        if (propItr->name.IsString() && std::strcmp(propItr->name.GetString(), "type") == 0) {
                            continue;
                        }
                        if (propValue.IsDouble()) {
                            luaRef[propName] = propValue.GetFloat();
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
                    actor->InjectConvenienceReferences(std::make_shared<luabridge::LuaRef>(luaRef));
                    if (luaRef["Ready"].isFunction()) {
                        luaRef["Ready"](luaRef);
                    }
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
            /*if (a.HasMember("view_pivot_offset_x") && a["view_pivot_offset_x"].IsNumber()) {
                if (a["view_pivot_offset_x"].IsInt()) {
                    actor->view_pivot_offset_x = a["view_pivot_offset_x"].GetInt();
                }
                else {
                    actor->view_pivot_offset_x = a["view_pivot_offset_x"].GetDouble();
                }
            }
            else if (actor->view_pivot_offset_x == 0) {
                int width, height;
                SDL_QueryTexture(imageDB.GetImage(actor->view_image), NULL, NULL, &width, &height);
                actor->view_pivot_offset_x = width * 0.5;
            }
            if (a.HasMember("view_pivot_offset_y") && a["view_pivot_offset_y"].IsNumber()) {
                if (a["view_pivot_offset_y"].IsInt()) {
                    actor->view_pivot_offset_y = a["view_pivot_offset_y"].GetInt();
                }
                else {
                    actor->view_pivot_offset_y = a["view_pivot_offset_y"].GetDouble();
                }
            }
            else if (actor->view_pivot_offset_y == 0) {
                int width, height;
                SDL_QueryTexture(imageDB.GetImage(actor->view_image), NULL, NULL, &width, &height);
                actor->view_pivot_offset_y = height * 0.5;
            }*/
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
            if (actor->name == "player") {
                playerPosition = glm::vec2(actor->transform_position_x, actor->transform_position_y);
            }
            actor->actor_id = id;
            id++;
            for (auto it = actor->components.begin(); it != actor->components.end(); it++) {
                luabridge::LuaRef luaRef = *it->second;
                if (luaRef["OnCollisionEnter"].isFunction()) {
                    actor->componentsWithCollideEnter.push_back(std::make_shared<luabridge::LuaRef>(luaRef));
                }
                if (luaRef["OnCollisionExit"].isFunction()) {
                    actor->componentsWithCollideExit.push_back(std::make_shared<luabridge::LuaRef>(luaRef));
                }
                if (luaRef["OnTriggerEnter"].isFunction()) {
                    actor->componentsWithTriggerEnter.push_back(std::make_shared<luabridge::LuaRef>(luaRef));
                }
                if (luaRef["OnTriggerExit"].isFunction()) {
                    actor->componentsWithTriggerExit.push_back(std::make_shared<luabridge::LuaRef>(luaRef));
                }
            }
            actors.push_back(actor);
        }
    }
}

const std::vector<Actor*>& SceneDB::getActors() const {
    return actors;
}

const glm::vec2 SceneDB::getPlayerPosition() const {
    return playerPosition;
}

std::string SceneDB::obtain_word_after_phrase(const std::string& input, const std::string& phrase) {
    size_t pos = input.find(phrase);
    if (pos == std::string::npos) return "";
    pos += phrase.length();
    while (pos < input.size() && std::isspace(input[pos])) {
        ++pos;
    }
    if (pos == input.size()) {
        return "";
    }
    size_t endPos = pos;
    while (endPos < input.size() && !std::isspace(input[endPos])) {
        ++endPos;
    }
    return input.substr(pos, endPos - pos);
}

bool SceneDB::compareActorPtrById(const Actor* a, const Actor* b) {
    return a->actor_id < b->actor_id;
}

int SceneDB::renderMap(int& health, int& score, std::vector<int>& scoreUp, std::vector<int>& neabySfx, ImageDB& imageDB, std::vector<std::string>& dialogueText, int cameraWidth, int cameraHeight, bool& isPlayer, double zoomFctor, glm::vec2 current_cam_pos, std::string score_sfx, AudioDB audioDB) {
    bool win = false;
    bool lose = false;
    string nextScene = "";
    string phrase = "proceed to";
    vector<Actor*> contactActors;
    vector<Actor*> nearbyActors;
    dialogueText.clear();
    vector<Actor*> renderActor(actors);
    Actor* player = renderActor[0];
    std::sort(renderActor.begin(), renderActor.end(), &Actor::actorComparator);
    for (auto actor : renderActor) {
        if (actor->name == "player") {
            player = actor;
            for (auto contactActor : actor->colliding_actors_this_frame) {
                contactActors.push_back(contactActor);
            }
            for (auto nearbyActor : actor->triggered_actors_this_frame) {
                nearbyActors.push_back(nearbyActor);
            }
        }
        actor->colliding_actors_this_frame.clear();
    }
    std::sort(contactActors.begin(), contactActors.end(), [](const Actor* a, const Actor* b) {
        return a->actor_id < b->actor_id;
    });
    std::sort(nearbyActors.begin(), nearbyActors.end(), [](const Actor* a, const Actor* b) {
        return a->actor_id < b->actor_id;
    });

    for (const auto& actor : contactActors) {
        if (actor->contact_dialogue != "") {
            //dialogueText.push_back(actor->contact_dialogue);
            nextScene = obtain_word_after_phrase(actor->contact_dialogue, phrase);
            if (actor->contact_dialogue.find("health down") != string::npos) {
                if (health == 3) {
                    health--;
                    lastHDFrame = Helper::GetFrameNumber();
                    if (player->damage_sfx != "") {
                        audioDB.RenderAudio(Helper::GetFrameNumber() % 48 + 2, player->damage_sfx, 0);
                    }
                }
                else {
                    if ((Helper::GetFrameNumber() - lastHDFrame) >= 180) {
                        health--;
                        lastHDFrame = Helper::GetFrameNumber();
                        if (player->damage_sfx != "") {
                            audioDB.RenderAudio(Helper::GetFrameNumber() % 48 + 2, player->damage_sfx, 0);
                        }
                    }
                }
                if (health <= 0) {
                    lose = 1;
                }
            }
            else if (actor->contact_dialogue.find("score up") != string::npos) {
                auto it = find(scoreUp.begin(), scoreUp.end(), actor->actor_id);
                if (it == scoreUp.end()) {
                    score++;
                    scoreUp.push_back(actor->actor_id);
                    if (score_sfx != "") {
                        audioDB.RenderAudio(1, score_sfx, 0);
                    }
                }
            }
            else if (actor->contact_dialogue.find("you win") != string::npos) {
                win = true;
            }
            else if (actor->contact_dialogue.find("game over") != string::npos) {
                lose = true;
            }
        }
    }

    for (const auto& actor : nearbyActors) {
        if (actor->nearby_dialogue != "") {
            auto it = find(neabySfx.begin(), neabySfx.end(), actor->actor_id);
            if (it == neabySfx.end()) {
                if (actor->nearby_dialogue_sfx != "") {
                    audioDB.RenderAudio(Helper::GetFrameNumber() % 48 + 2, actor->nearby_dialogue_sfx, 0);
                }
                neabySfx.push_back(actor->actor_id);
            }
            dialogueText.push_back(actor->nearby_dialogue);
            nextScene = obtain_word_after_phrase(actor->nearby_dialogue, phrase);
            if (actor->nearby_dialogue.find("health down") != string::npos) {
                if (health == 3) {
                    health--;
                    lastHDFrame = Helper::GetFrameNumber();
                    if (player->damage_sfx != "") {
                        audioDB.RenderAudio(Helper::GetFrameNumber() % 48 + 2, player->damage_sfx, 0);
                    }
                }
                else {
                    if ((Helper::GetFrameNumber() - lastHDFrame) >= 180) {
                        health--;
                        lastHDFrame = Helper::GetFrameNumber();
                        if (player->damage_sfx != "") {
                            audioDB.RenderAudio(Helper::GetFrameNumber() % 48 + 2, player->damage_sfx, 0);
                        }
                    }
                }
                if (health <= 0) {
                    lose = 1;
                }
            }
            else if (actor->nearby_dialogue.find("score up") != string::npos) {
                auto it = find(scoreUp.begin(), scoreUp.end(), actor->actor_id);
                if (it == scoreUp.end()) {
                    score++;
                    scoreUp.push_back(actor->actor_id);
                    if (score_sfx != "") {
                        audioDB.RenderAudio(1, score_sfx, 0);
                    }
                }
            }
            else if (actor->nearby_dialogue.find("you win") != string::npos) {
                win = true;
            }
            else if (actor->nearby_dialogue.find("game over") != string::npos) {
                lose = true;
            }
        }
    }
    for (auto actor : renderActor) {
        imageDB.RenderActorImage(actor, cameraWidth, cameraHeight, zoomFctor, current_cam_pos, lastHDFrame);
    }
    //ss << "health : " << health << ", score : " << score << '\n';
    //
    //string output = ss.str();
    
    //cout << output;
    if (nextScene != "") {
        //loadScene(nextScene, imageDB, isPlayer);
        return 3;
    }
    if (win) {
        return 1;
    }
    else if (lose) {
        return 2;
    }
    return 0;
}

void SceneDB::handleInput(glm::vec2 playerDirection, double player_movement_speed, double& newPlayerX, double& newPlayerY, AudioDB audioDB) {
    Actor* actor = actors[0];
    for (auto player : actors) {
        if (player->name == "player") {
            actor = player;
        }
    }
    double newX = actor->transform_position_x + double(playerDirection.x) * player_movement_speed;
    double newY = actor->transform_position_y + double(playerDirection.y) * player_movement_speed;
    newPlayerX = newX;
    newPlayerY = newY;
    if (Helper::GetFrameNumber() % 20 == 0) {
        if (actor->step_sfx != "") {
            audioDB.RenderAudio(Helper::GetFrameNumber() % 48 + 2, actor->step_sfx, 0);
        }
    }
}

//void SceneDB::updateActors(double newPlayerX, double newPlayerY, bool x_scale_actor_flipping_on_movement) {
//    for (auto& actor : actors) {
//        double newX = actor->transform_position_x + actor->vel_x;
//        double newY = actor->transform_position_y + actor->vel_y;
//        if (actor->name == "player") {
//            newX = newPlayerX;
//            newY = newPlayerY;
//        }
//        glm::vec2 newLocation(newX, newY);
//        glm::vec2 oldLocation(actor->transform_position_x, actor->transform_position_y);
//        if (newLocation != oldLocation) {
//            actor->is_moving = true;
//        }
//        else {
//            actor->is_moving = false;
//        }
//        if (actor->box_collider_height != 0 && actor->box_collider_width != 0) {
//            float left = newLocation.x - actor->box_collider_width * 0.5f;
//            float right = newLocation.x + actor->box_collider_width * 0.5f;
//            float top = newLocation.y - actor->box_collider_height * 0.5f;
//            float bottom = newLocation.y + actor->box_collider_height * 0.5f;
//            for (auto& otherActor : actors) {
//                if (otherActor->name != actor->name && otherActor->box_collider_height != 0 && otherActor->box_collider_width != 0) {
//                    float second_left = otherActor->transform_position_x - otherActor->box_collider_width * 0.5f;
//                    float second_right = otherActor->transform_position_x + otherActor->box_collider_width * 0.5f;
//                    float second_top = otherActor->transform_position_y - otherActor->box_collider_height * 0.5f;
//                    float second_bottom = otherActor->transform_position_y + otherActor->box_collider_height * 0.5f;
//                    if (left < second_right && right > second_left && top < second_bottom && bottom > second_top) {
//                        actor->colliding_actors_this_frame.emplace(otherActor);
//                        otherActor->colliding_actors_this_frame.emplace(actor);
//                    }
//                }
//            }
//        }
//        if (actor->box_trigger_height != 0 && actor->box_trigger_width != 0) {
//            float left = newLocation.x - actor->box_trigger_width * 0.5f;
//            float right = newLocation.x + actor->box_trigger_width * 0.5f;
//            float top = newLocation.y - actor->box_trigger_height * 0.5f;
//            float bottom = newLocation.y + actor->box_trigger_height * 0.5f;
//            for (auto& otherActor : actors) {
//                if (otherActor->name != actor->name && otherActor->box_trigger_height != 0 && otherActor->box_trigger_width != 0) {
//                    float second_left = otherActor->transform_position_x - otherActor->box_trigger_width * 0.5f;
//                    float second_right = otherActor->transform_position_x + otherActor->box_trigger_width * 0.5f;
//                    float second_top = otherActor->transform_position_y - otherActor->box_trigger_height * 0.5f;
//                    float second_bottom = otherActor->transform_position_y + otherActor->box_trigger_height * 0.5f;
//                    if (left < second_right && right > second_left && top < second_bottom && bottom > second_top) {
//                        actor->triggered_actors_this_frame.emplace(otherActor);
//                        otherActor->triggered_actors_this_frame.emplace(actor);
//                    }
//                }
//            }
//        }
//        bool isOccupied = false;
//        if (actor->colliding_actors_this_frame.size() > 0) {
//            isOccupied = true;
//        }
//        if (!isOccupied) {
//            actor->transform_position_x = newX;
//            actor->transform_position_y = newY;
//            if (actor->name == "player") {
//                playerPosition = glm::vec2(newX, newY);
//            }
//        }
//        else {
//            // Reverse velocity if the new position is blocked
//            actor->vel_x *= -1;
//            actor->vel_y *= -1;
//        }
//        if (x_scale_actor_flipping_on_movement) {
//            if ((newLocation.x > oldLocation.x && actor->transform_scale_x < 0) || (newLocation.x < oldLocation.x && actor->transform_scale_x > 0)) {
//                actor->transform_scale_x = -actor->transform_scale_x;
//            }
//        }
//        if (actor->view_image_back != "") {
//            if (newLocation.y < oldLocation.y) {
//                actor->moving_up = true;
//            }
//            if (newLocation.y > oldLocation.y) {
//                actor->moving_up = false;
//            }
//        }
//    }
//}
