#ifndef TEXTDB_H
#define TEXTDB_H

#include <unordered_map>
#include <vector>
#include <string>
#include "../SDL2/SDL.h"
#include "../SDL2_ttf/SDL_ttf.h"

class TextDB {
public:
    TextDB(SDL_Renderer* renderer);
    ~TextDB();

    void LoadFontsFromDirectory(const std::string& directoryPath);
    void CreateTextureFromText(const std::string& text, const std::string& fontName, int fontSize, SDL_Color textColor);
    void AddTextTexture(SDL_Texture* texture);
    void RenderText(int index, int x, int y);
    void RenderTextFromString(const std::string& text, const std::string& fontName, int fontSize, SDL_Color textColor, int x, int y);

    static void Draw(std::string str_content, int x, int y, std::string font_name, int font_size, int r, int g, int b, int a);
private:
    SDL_Renderer* renderer;
    static inline std::unordered_map<std::string, TTF_Font*> fonts;
    std::vector<SDL_Texture*> textTextures;
    std::unordered_map<std::string, std::unordered_map<int, TTF_Font*>> usedFonts;
};

#endif // TEXTDB_H
