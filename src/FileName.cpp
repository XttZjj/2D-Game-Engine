#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <sstream>
#include <filesystem>
#include <fstream>
//#include "MapHelper.h"
#include "../Third Party/glm/glm/glm.hpp"
#pragma warning(disable: 4996)
#include "../Third Party/rapidjson-1.1.0/include/rapidjson/document.h"
#include "../Third Party/rapidjson-1.1.0/include/rapidjson/filereadstream.h"
#include "Rigidbody.h"
#include "SceneDB.h"
#include "AudioDB.h"
#include "Helper.h"
#include "AudioHelper.h"
#include "Input.h"
#include "../SDL2/SDL.h"
#include "../SDL2_image/SDL_image.h"
#include "../SDL2_ttf/SDL_ttf.h"
#include "../SDL2_mixer/SDL_mixer.h"
#include "lua.hpp"
#include "LuaBridge.h"
#include "Renderer.h"
#include "box2d/box2d.h"
#include "Event.h"

using namespace std;

const string GAME_CONFIG_FILE = "resources/game.config";
const string RENDERING_CONFIG_FILE = "resources/rendering.config";

string game_over_good_image;
string game_over_bad_image;
string game_over_bad_audio;
string game_over_good_audio;

string game_title = "";
string font_name = "";

vector<string> intro_image;
vector<string> intro_text;
string intro_bgm;
string gameplay_bgm;
string hp_image;
string score_sfx = "";
double player_movement_speed = 0.02;


bool has_over_good_image = false;
bool has_over_bad_image = false;
bool has_over_good_audio = false;
bool has_over_bad_audio = false;
bool has_images = false;
bool has_font = false;
bool has_text = false;
bool has_bgm = false;
bool has_gameplay_bgm = false;
bool has_hp = false;

string initialScene;

int cameraWidth = 640;
int cameraHeight = 360;
int clear_color_r = 255;
int clear_color_g = 255;
int clear_color_b = 255;
double cam_offset_x = 0;
double cam_offset_y = 0;
double zoom_factor = 1;
double cam_ease_factor = 1;
bool x_scale_actor_flipping_on_movement = false;

bool win = false;
bool lose = false;

void checkResourcesDirectory() {
    if (!filesystem::exists("resources")) {
        cout << "error: resources/ missing";
        exit(0);
    }
}

void checkGameConfigFile() {
    if (!filesystem::exists(GAME_CONFIG_FILE)) {
        cout << "error: " << GAME_CONFIG_FILE << " missing";
        exit(0);
    }
}

void checkSceneFile(string level) {
    string scenePath = "resources/scenes/" + level + ".scene";
    if (!filesystem::exists(scenePath)) {
        cout << "error: scene " << level << " is missing";
        exit(0);
    }
}

void ReadJsonFile(const std::string& path, rapidjson::Document& out_document) {
    FILE* file_pointer = nullptr;
#ifdef _WIN32
    fopen_s(&file_pointer, path.c_str(), "rb");
#else
    file_pointer = fopen(path.c_str(), "rb");
#endif
    char buffer[65536];
    rapidjson::FileReadStream stream(file_pointer, buffer, sizeof(buffer));
    out_document.ParseStream(stream);
    std::fclose(file_pointer);

    if (out_document.HasParseError()) {
        rapidjson::ParseErrorCode errorCode = out_document.GetParseError();
        std::cout << "error parsing json at [" << path << "]" << std::endl;
        exit(0);
    }
}

void startGame() {
    checkResourcesDirectory();
    checkGameConfigFile();

    rapidjson::Document configDocument;

    ReadJsonFile(GAME_CONFIG_FILE, configDocument);

    if (configDocument.HasMember("game_over_bad_image")) {
        game_over_bad_image = configDocument["game_over_bad_image"].GetString();
        has_over_bad_image = true;
    }

    if (configDocument.HasMember("game_over_good_image")) {
        game_over_good_image= configDocument["game_over_good_image"].GetString();
        has_over_good_image = true;
    }

    if (configDocument.HasMember("game_over_bad_audio")) {
        game_over_bad_audio = configDocument["game_over_bad_audio"].GetString();
        has_over_bad_audio = true;
    }

    if (configDocument.HasMember("game_over_good_audio")) {
        game_over_good_audio = configDocument["game_over_good_audio"].GetString();
        has_over_good_audio = true;
    }

    if (configDocument.HasMember("game_title")) {
        game_title = configDocument["game_title"].GetString();
    }

    if (configDocument.HasMember("gameplay_audio")) {
        gameplay_bgm = configDocument["gameplay_audio"].GetString();
        has_gameplay_bgm = true;
    }

    if (configDocument.HasMember("score_sfx")) {
        score_sfx = configDocument["score_sfx"].GetString();
    }
    
    if (configDocument.HasMember("hp_image")) {
        hp_image = configDocument["hp_image"].GetString();
        has_hp = true;
    }

    if (configDocument.HasMember("player_movement_speed")) {
        if (configDocument["player_movement_speed"].IsInt()) {
            player_movement_speed = configDocument["player_movement_speed"].GetInt();
        }
        else if (configDocument["player_movement_speed"].IsDouble()) {
            player_movement_speed = configDocument["player_movement_speed"].GetDouble();
        }
    }

    if (configDocument.HasMember("intro_image") && configDocument["intro_image"].IsArray()) {
        for (const auto& image : configDocument["intro_image"].GetArray()) {
            intro_image.push_back(image.GetString());
        }
        has_images = true;
    }

    if (configDocument.HasMember("font")) {
        font_name = configDocument["font"].GetString();
        has_font = true;
    }

    if (has_images) {
        if (configDocument.HasMember("intro_text") && configDocument["intro_text"].IsArray()) {
            if (!has_font) {
                cout << "error: text render failed. No font configured";
                exit(0);
            }
            for (const auto& text : configDocument["intro_text"].GetArray()) {
                intro_text.push_back(text.GetString());
            }
            has_text = true;
        }
    }

    if (configDocument.HasMember("intro_bgm")) {
        has_bgm = true;
        intro_bgm = configDocument["intro_bgm"].GetString();
    }

    if (configDocument.HasMember("initial_scene")) {
        initialScene = configDocument["initial_scene"].GetString();
    }
    else {
        cout << "error: initial_scene unspecified";
        exit(0);
    }

    if (filesystem::exists(RENDERING_CONFIG_FILE)) {
        ReadJsonFile(RENDERING_CONFIG_FILE, configDocument);

        if (configDocument.HasMember("x_resolution")) {
            cameraWidth = configDocument["x_resolution"].GetInt();
        }

        if (configDocument.HasMember("y_resolution")) {
            cameraHeight = configDocument["y_resolution"].GetInt();
        }

        if (configDocument.HasMember("clear_color_r")) {
            clear_color_r = configDocument["clear_color_r"].GetInt();
        }

        if (configDocument.HasMember("clear_color_g")) {
            clear_color_g = configDocument["clear_color_g"].GetInt();
        }

        if (configDocument.HasMember("clear_color_b")) {
            clear_color_b = configDocument["clear_color_b"].GetInt();
        }

        if (configDocument.HasMember("cam_offset_x")) {
            if (configDocument["cam_offset_x"].IsInt()) {
                cam_offset_x = configDocument["cam_offset_x"].GetInt();
            }
            else if (configDocument["cam_offset_x"].IsDouble()) {
                cam_offset_x = configDocument["cam_offset_x"].GetDouble();
            }
        }

        if (configDocument.HasMember("cam_offset_y")) {
            if (configDocument["cam_offset_y"].IsInt()) {
                cam_offset_y = configDocument["cam_offset_y"].GetInt();
            }
            else if (configDocument["cam_offset_y"].IsDouble()) {
                cam_offset_y = configDocument["cam_offset_y"].GetDouble();
            }
        }

        if (configDocument.HasMember("zoom_factor")) {
            if (configDocument["zoom_factor"].IsInt()) {
                zoom_factor = configDocument["zoom_factor"].GetInt();
            }
            else if (configDocument["zoom_factor"].IsDouble()) {
                zoom_factor = configDocument["zoom_factor"].GetDouble();
            }
        }

        if (configDocument.HasMember("cam_ease_factor")) {
            if (configDocument["cam_ease_factor"].IsInt()) {
                cam_ease_factor = configDocument["cam_ease_factor"].GetInt();
            }
            else if (configDocument["cam_ease_factor"].IsDouble()) {
                cam_ease_factor = configDocument["cam_ease_factor"].GetDouble();
            }
        }

        if (configDocument.HasMember("x_scale_actor_flipping_on_movement")) {
            x_scale_actor_flipping_on_movement = configDocument["x_scale_actor_flipping_on_movement"].GetBool();
        }
    }
}

int main(int argc, char* argv[]) {
    bool endGame = false;
    startGame();
    SceneDB sceneDB;
    SDL_Init(SDL_INIT_GAMECONTROLLER);
    for (int i = 0; i < SDL_NumJoysticks(); ++i) {
        if (SDL_IsGameController(i)) {
            std::cout << "Button A was pressed." << std::endl;
            SDL_GameController* controller = SDL_GameControllerOpen(i);
            if (controller == nullptr) {
                SDL_Log("Could not open gamecontroller %i: %s", i, SDL_GetError());
            }
        }
    }
    SDL_Window* window = Helper::SDL_CreateWindow498(game_title.c_str(), 100, 100, cameraWidth, cameraHeight, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = Helper::SDL_CreateRenderer498(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor(renderer, clear_color_r, clear_color_g, clear_color_b, 255);
    SDL_RenderClear(renderer);
    TTF_Init();
    ComponentManager componentManager;
    componentManager.Initialize();
    Renderer gameRenderer(renderer, cameraWidth, cameraHeight);
    Renderer::zoomFactor = 1;
    Renderer::cameraPosition = glm::vec2(0, 0);
    ComponentManager::changeScene = false;
    ComponentManager::nextSceneName = "";
    AudioDB audioDB;
    AudioHelper::Mix_AllocateChannels498(50);
    if (filesystem::exists("resources/audio/")) {
        audioDB.LoadAudioFromDirectory("resources/audio/");
    }
    if (filesystem::exists("resources/images/")) {
        gameRenderer.LoadImagesFromDirectory("resources/images/");
    }
    sceneDB.loadScene(initialScene);
    ComponentManager::currentScene = initialScene;
    for (auto actor : SceneDB::actors) {
        actor->RunOnStart();
    }
    Input::Init();
    while (true) {
        SDL_Event e;
        while (Helper::SDL_PollEvent498(&e)) {
            if (e.type == SDL_QUIT) {
                endGame = true;
            }
            Input::ProcessEvent(e);
        }
        SDL_SetRenderDrawColor(renderer, clear_color_r, clear_color_g, clear_color_b, 255);
        SDL_RenderClear(renderer);
        if (ComponentManager::changeScene) {
            for (auto it = SceneDB::actors.begin(); it != SceneDB::actors.end();) {
                if (!(*it)->dontDestroy) {
                    (*it)->RunOnDestroy();
                    delete* it;
                    it = SceneDB::actors.erase(it);
                }
                else {
                    ++it;
                }
            }
            sceneDB.loadScene(ComponentManager::nextSceneName);
            ComponentManager::currentScene = ComponentManager::nextSceneName;
            ComponentManager::changeScene = false;
            for (auto actor : SceneDB::actors) {
                if (!actor->dontDestroy)
                    actor->RunOnStart();
            }
        }
        for (auto actor : SceneDB::ActorsToAdd) {
            SceneDB::actors.push_back(actor);
            for (auto it : actor->components) {
                luabridge::LuaRef luaRef = *it.second;
                if (luaRef["Ready"].isFunction()) {
                    luaRef["Ready"](luaRef);
                }
            }
            actor->RunOnStart();
        }
        SceneDB::ActorsToAdd.clear();
        for (auto actor : SceneDB::actors) {
            actor->RunAddOnStart();
        }
        ComponentManager::nAddStart = ComponentManager::nAdd;
        for (auto actor : SceneDB::actors) {
            actor->RunOnUpdate();
        }
        for (auto actor : SceneDB::actors) {
            actor->RunOnLateUpdate();
        }
        for (auto actor : SceneDB::ActorsToRemove) {
            actor->RunOnDestroy();
        }
        SceneDB::actors.erase(std::remove_if(SceneDB::actors.begin(), SceneDB::actors.end(),
            [](Actor* actor) {
                return std::find(SceneDB::ActorsToRemove.begin(), SceneDB::ActorsToRemove.end(), actor) != SceneDB::ActorsToRemove.end();
            }), SceneDB::actors.end());
        SceneDB::ActorsToRemove.clear();
        Event::ProcessQueuedRequests();
        gameRenderer.RenderRequests();
        Input::LateUpdate();
        Rigidbody::WorldUpdate();
        Helper::SDL_RenderPresent498(renderer);
        if (endGame) {
            exit(0);
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    return 0;
}
