#ifndef IMAGEDB_H
#define IMAGEDB_H

#include <unordered_map>
#include <string>
#include "Actor.h"
#include "../SDL2/SDL.h"
#include "../Third Party/glm/glm/glm.hpp"
#include "Helper.h"

class ImageDB {
public:
    ImageDB(SDL_Renderer* renderer); // Constructor with renderer parameter
    ~ImageDB();

    void LoadImagesFromDirectory(const std::string& directoryPath);
    SDL_Texture* GetImage(const std::string& name);
    void RenderIntroImage(const std::string& name);
    void RenderHealthImage(const std::string& name, int index);
    void RenderActorImage(const Actor* actor, int window_width, int window_height, double zoomFactor, glm::vec2 current_cam_pos, int lastHDFrame);

private:
    SDL_Renderer* renderer; // Renderer used for creating textures
    std::unordered_map<std::string, SDL_Texture*> images;
};

#endif // IMAGEDB_H

