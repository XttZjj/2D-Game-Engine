#include "TextDB.h"
#include <iostream>
#include <filesystem>

TextDB::TextDB(SDL_Renderer* renderer)
    : renderer(renderer) {}

TextDB::~TextDB() {
    // Clean up loaded fonts
    for (auto& pair : fonts) {
        TTF_CloseFont(pair.second);
    }
    fonts.clear();

    // Clean up text textures
    for (auto& texture : textTextures) {
        SDL_DestroyTexture(texture);
    }
    textTextures.clear();
}

void TextDB::LoadFontsFromDirectory(const std::string& directoryPath) {
    if (std::filesystem::exists(directoryPath)) {
        for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".ttf") {
                // Load the font file
                TTF_Font* font = TTF_OpenFont(entry.path().string().c_str(), 16);
                std::string fontName = entry.path().filename().stem().string();
                fonts[fontName] = font;
            }
        }
    }
}

void TextDB::CreateTextureFromText(const std::string& text, const std::string& fontName, int fontSize, SDL_Color textColor) {
    auto it = fonts.find(fontName);
    if (it != fonts.end()) {
        // Render text to surface
        TTF_Font* font = it->second;
        SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), textColor);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        textTextures.push_back(texture);
        return;
    }
    std::cout << "error: font " << fontName << " missing";
    exit(0);
}

void TextDB::AddTextTexture(SDL_Texture* texture) {
    // Add the text texture to the vector
    textTextures.push_back(texture);
}

void TextDB::RenderText(int index, int x, int y) {
    if (index >= 0 && index < textTextures.size()) {
        SDL_Texture* texture = textTextures[index];
        int width, height;
        SDL_QueryTexture(texture, NULL, NULL, &width, &height);
        SDL_Rect destRect = { x, y, width, height };
        SDL_RenderCopy(renderer, texture, NULL, &destRect);
    }
}

void TextDB::RenderTextFromString(const std::string& text, const std::string& fontName, int fontSize, SDL_Color textColor, int x, int y) {
    auto it = fonts.find(fontName);
    if (it != fonts.end()) {
        // Render text to surface
        TTF_Font* font = it->second;
        SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), textColor);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        int width, height;
        SDL_QueryTexture(texture, NULL, NULL, &width, &height);
        SDL_Rect destRect = { x, y, width, height };
        SDL_RenderCopy(renderer, texture, NULL, &destRect);
    }
}
