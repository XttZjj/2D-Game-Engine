#ifndef SCENE_DB_H
#define SCENE_DB_H

#include <vector>
#include <string>
#include <unordered_map>
#include "../Third Party/glm/glm/glm.hpp"
#include "../SDL2/SDL.h"
#include "ImageDB.h"
#include "TextDB.h"
#include "Helper.h"
#include "AudioDB.h"
#include "ComponentManager.h"

struct IVec2Hash {
    std::size_t operator()(const glm::ivec2& vec) const {
        // Combine the hash values of the x and y components
        return std::hash<int>()(vec.x) ^ std::hash<int>()(vec.y);
    }
};

class SceneDB {
private:
    glm::vec2 playerPosition = glm::vec2(0,0);
    int lastHDFrame = -1;
public:
    SceneDB();

    static inline std::vector<Actor*> actors;
    // Function to load a scene from a JSON file
    void loadScene(const std::string& filename);

    // Function to retrieve the loaded actors
    const std::vector<Actor*>& getActors() const;

    const glm::vec2 getPlayerPosition() const;

    int renderMap(int& health, int& score, std::vector<int>& scoreUp, std::vector<int>& neabySfx, ImageDB& imageDB, std::vector<std::string>& dialogueText, int cameraWidth, int cameraHeight, bool& isPlayer, double zoomFactor, glm::vec2 current_cam_pos, std::string score_sfx, AudioDB audioDB);

    void handleInput(glm::vec2 playerDirection, double player_movement_speed, double& newPlayerX, double& newPlayerY, AudioDB audioDB);

    std::string obtain_word_after_phrase(const std::string& input, const std::string& phrase);

    bool compareActorPtrById(const Actor* a, const Actor* b);

    void updateActors(double newPlayerX, double newPlayerY, bool x_scale_actor_flipping_on_movement);

    static inline std::vector<Actor*> ActorsToAdd;

    static inline std::vector<Actor*> ActorsToRemove;
};


#endif // SCENE_DB_H
