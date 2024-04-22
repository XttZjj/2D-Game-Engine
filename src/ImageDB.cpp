// ImageDB.cpp
#include "ImageDB.h"
#include <filesystem>
#include <iostream>
#include <cmath>
#include "../SDL2/SDL.h"
#include "../SDL2_image/SDL_image.h"

ImageDB::ImageDB(SDL_Renderer* renderer)
    : renderer(renderer) {}

ImageDB::~ImageDB() {
    // Clean up loaded images
    for (auto& pair : images) {
        SDL_DestroyTexture(pair.second);
    }
    images.clear();
}

void ImageDB::LoadImagesFromDirectory(const std::string& directoryPath) {
    if (std::filesystem::exists(directoryPath)) {
        for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".png") {
                SDL_Texture* texture = IMG_LoadTexture(renderer, entry.path().string().c_str());
                std::string fileName = entry.path().filename().stem().string();
                images[fileName] = texture;
            }
        }
    }
}

SDL_Texture* ImageDB::GetImage(const std::string& name) {
    auto it = images.find(name);
    if (it != images.end()) {
        return it->second;
    }
    std::cout << "error: missing image " << name;
    exit(0);
    return nullptr;
}

void ImageDB::RenderIntroImage(const std::string& name) {
    SDL_Texture* texture = GetImage(name);
    if (texture) {
        SDL_RenderCopy(renderer, texture, NULL, NULL);
    }
}

void ImageDB::RenderHealthImage(const std::string& name, int index) {
    int width, height;
    SDL_QueryTexture(GetImage(name), NULL, NULL, &width, &height);
    SDL_Rect destinationRect;
    destinationRect.x = 5 + index * (width + 5);
    destinationRect.y = 25;
    destinationRect.w = width;
    destinationRect.h = height;
    SDL_RenderCopy(renderer, GetImage(name), NULL, &destinationRect);
}

void ImageDB::RenderActorImage(const Actor* actor, int window_width, int window_height, double zoomFactor, glm::vec2 current_cam_pos, int lastHDFrame) {
    std::string image_to_show = actor->view_image;
    glm::vec2 extra_view_offset = glm::vec2(0, 0);
    if (actor->movement_bounce_enabled && actor->is_moving) {
        extra_view_offset = glm::vec2(0, -glm::abs(glm::sin(Helper::GetFrameNumber() * 0.15f)) * 10.0f);
    }
    if (actor->view_image_back != "" && actor->moving_up) {
        image_to_show = actor->view_image_back;
    }
    if (actor->name == "player" && lastHDFrame != -1 && Helper::GetFrameNumber() - lastHDFrame < 30) {
        if (actor->view_image_damage != "") {
            image_to_show = actor->view_image_damage;
        }
    }
    if (actor->name != "player" && lastHDFrame != -1 && Helper::GetFrameNumber() - lastHDFrame < 30) {
        if (actor->view_image_attack != "") {
            image_to_show = actor->view_image_attack;
        }
    }
    int width, height;
    SDL_QueryTexture(GetImage(image_to_show), NULL, NULL, &width, &height);
    SDL_Point pivotSDLPoint;
    pivotSDLPoint.x = std::round(actor->view_pivot_offset_x) * std::abs(actor->transform_scale_x);
    pivotSDLPoint.y = std::round(actor->view_pivot_offset_y) * std::abs(actor->transform_scale_y);

    SDL_Rect destinationRect;
    destinationRect.x = std::round((actor->transform_position_x - current_cam_pos.x) * 100 + window_width * 0.5f / zoomFactor - pivotSDLPoint.x + extra_view_offset.x);
    destinationRect.y = std::round((actor->transform_position_y - current_cam_pos.y) * 100 + window_height * 0.5f / zoomFactor - pivotSDLPoint.y + extra_view_offset.y);
    destinationRect.w = width * std::abs(actor->transform_scale_x);
    destinationRect.h = height * std::abs(actor->transform_scale_y);
    // Calculate rotation angle in degrees
    double angle = actor->transform_rotation_degrees;

    // Render the actor's image with rotation
    /*if (actor->transform_scale_x >= 0 && actor->transform_scale_y >= 0) {
        Helper::SDL_RenderCopyEx498(renderer, GetImage(image_to_show), NULL, &destinationRect, angle, &pivotSDLPoint, SDL_FLIP_NONE);
    }
    else if (actor->transform_scale_x < 0 && actor->transform_scale_y >= 0) {
        SDL_RenderCopyEx(renderer, GetImage(image_to_show), NULL, &destinationRect, angle, &pivotSDLPoint, SDL_FLIP_HORIZONTAL);
    }
    else if (actor->transform_scale_x >= 0 && actor->transform_scale_y < 0) {
        SDL_RenderCopyEx(renderer, GetImage(image_to_show), NULL, &destinationRect, angle, &pivotSDLPoint, SDL_FLIP_VERTICAL);
    }
    else {
        SDL_RendererFlip flip = static_cast<SDL_RendererFlip>(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL);
        SDL_RenderCopyEx(renderer, GetImage(image_to_show), NULL, &destinationRect, angle, &pivotSDLPoint, flip);
    }*/
}
