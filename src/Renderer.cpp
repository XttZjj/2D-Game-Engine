#include "Renderer.h"

Renderer::Renderer(SDL_Renderer* renderer, int cameraWidth, int cameraHeight)
    : renderer(renderer), cameraWidth(cameraWidth), cameraHeight(cameraHeight) {}

Renderer::TextRenderRequest::TextRenderRequest()
    : text(""), font(""), color({ 0, 0, 0, 255 }), size(0), x(0), y(0) {}

Renderer::TextRenderRequest::TextRenderRequest(std::string _text, std::string _font,
    SDL_Color _color, int _size, int _x, int _y)
    : text(_text), font(_font), color(_color), size(_size), x(_x), y(_y) {}

Renderer::ImageRenderRequest::ImageRenderRequest() 
    : image(""), color({ 0, 0, 0, 255 }), x(0), y(0), sorting_order(0), rotation(0), scale_x(0), scale_y(0), pivot_x(0), pivot_y(0) {};
Renderer::ImageRenderRequest::ImageRenderRequest(std::string image, SDL_Color color, float x, float y, int sorting_order,
    float rotation, float scale_x, float scale_y, float pivot_x, float pivot_y)
    : image(image), color(color), x(x), y(y), sorting_order(sorting_order), rotation(rotation),
    scale_x(scale_x), scale_y(scale_y), pivot_x(pivot_x), pivot_y(pivot_y) {}

Renderer::UIRenderRequest::UIRenderRequest()
    : image(""), color({ 0, 0, 0, 255 }), x(0), y(0), sorting_order(0) {};
Renderer::UIRenderRequest::UIRenderRequest(std::string image, SDL_Color color, float x, float y, int sorting_order)
    : image(image), color(color), x(x), y(y), sorting_order(sorting_order) {}

Renderer::DrawPixelRequest::DrawPixelRequest()
    : color({ 0, 0, 0, 255 }), x(0), y(0) {};
Renderer::DrawPixelRequest::DrawPixelRequest(SDL_Color color, float x, float y)
    : color(color), x(x), y(y) {}

void Renderer::TextDraw(std::string text, float x, float y, std::string font, float size, float r, float g, float b, float a) {
    SDL_Color color = { static_cast<Uint8>(r), static_cast<Uint8>(g), static_cast<Uint8>(b), static_cast<Uint8>(a) };
    TextQue.push(TextRenderRequest(text, font, color, size, x, y));
}

void Renderer::UIDraw(std::string image_name, float x, float y) {
    SDL_Color color = { 255, 255, 255, 255 };
    UIQue.push(UIRenderRequest(image_name, color, x, y, 0));
}

void Renderer::UIDrawEx(std::string image_name, float x, float y, float r, float g, float b, float a, int sorting_order) {
    SDL_Color color = { static_cast<Uint8>(r), static_cast<Uint8>(g), static_cast<Uint8>(b), static_cast<Uint8>(a) };
    UIQue.push(UIRenderRequest(image_name, color, x, y, sorting_order));
}

void Renderer::ImageDraw(std::string image_name, float x, float y) {
    SDL_Color color = { 255, 255, 255, 255 };
    ImageQue.push(ImageRenderRequest(image_name, color, x, y, 0, 0, 1, 1, 0.5f, 0.5f));
}

void Renderer::ImageDrawEX(std::string image_name, float x, float y, float rotation, float scale_x, float scale_y, float pivot_x, float pivot_y, float r, float g, float b, float a, int sorting_order) {
    SDL_Color color = { static_cast<Uint8>(r), static_cast<Uint8>(g), static_cast<Uint8>(b), static_cast<Uint8>(a) };
    ImageQue.push(ImageRenderRequest(image_name, color, x, y, sorting_order, rotation, scale_x, scale_y, pivot_x, pivot_y));
}

void Renderer::PixelDraw(float x, float y, float r, float g, float b, float a) {
    SDL_Color color = { static_cast<Uint8>(r), static_cast<Uint8>(g), static_cast<Uint8>(b), static_cast<Uint8>(a) };
    PixelQue.push(DrawPixelRequest(color, x, y));
}

bool CompareImageRequests(const Renderer::ImageRenderRequest& a, const Renderer::ImageRenderRequest& b) {
    return a.sorting_order < b.sorting_order;
}

bool CompareUIRequests(const Renderer::UIRenderRequest& a, const Renderer::UIRenderRequest& b) {
    return a.sorting_order < b.sorting_order;
}

void Renderer::RenderRequests() {
    std::vector<Renderer::ImageRenderRequest> tempVec;
    while (!ImageQue.empty()) {
        tempVec.push_back(ImageQue.front());
        ImageQue.pop();
    }
    std::stable_sort(tempVec.begin(), tempVec.end(), CompareImageRequests);
    for (const auto& item : tempVec) {
        ImageQue.push(item);
    }
    std::vector<Renderer::UIRenderRequest> uiVec;
    while (!UIQue.empty()) {
        uiVec.push_back(UIQue.front());
        UIQue.pop();
    }
    std::stable_sort(uiVec.begin(), uiVec.end(), CompareUIRequests);
    for (const auto& item : uiVec) {
        UIQue.push(item);
    }
    SDL_RenderSetScale(renderer, zoomFactor, zoomFactor);
    RenderImageRequest();
    SDL_RenderSetScale(renderer, 1, 1);
    RenderUIRequest();
    RenderTextRequest();
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    RenderPixelRequest();
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

void Renderer::RenderTextRequest() {
    while (!TextQue.empty()) {
        TextRenderRequest textRequest = TextQue.front();
        TextQue.pop();
        TTF_Font* font;
        if (TextRenderRequest::fonts.find(textRequest.font) != TextRenderRequest::fonts.end()) {
            if (TextRenderRequest::fonts[textRequest.font].find(textRequest.size) != TextRenderRequest::fonts[textRequest.font].end()) {
                font = TextRenderRequest::fonts[textRequest.font][textRequest.size];
            } else {
                font = TextRenderRequest::LoadFont(textRequest.font, textRequest.size);
            }
        }
        else {
            font = TextRenderRequest::LoadFont(textRequest.font, textRequest.size);
        }
        SDL_Surface* surface = TTF_RenderText_Solid(font, textRequest.text.c_str(), textRequest.color);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        int width, height;
        SDL_QueryTexture(texture, NULL, NULL, &width, &height);
        SDL_Rect destRect = { textRequest.x, textRequest.y, width, height };
        SDL_RenderCopy(renderer, texture, NULL, &destRect);
    }
}

void Renderer::RenderUIRequest() {
    while (!UIQue.empty()) {
        UIRenderRequest UIRequest = UIQue.front();
        UIQue.pop();
        SDL_Texture* tex = GetImage(UIRequest.image);
        SDL_Rect tex_rect;
        SDL_QueryTexture(tex, NULL, NULL, &tex_rect.w, &tex_rect.h);
        tex_rect.x = UIRequest.x;
        tex_rect.y = UIRequest.y;
        SDL_SetTextureColorMod(tex, UIRequest.color.r, UIRequest.color.g, UIRequest.color.b);
        SDL_SetTextureAlphaMod(tex, UIRequest.color.a);
        SDL_Point pivotSDLPoint;
        Helper::SDL_RenderCopyEx498(-1, "", renderer, tex, NULL, &tex_rect, 0, &pivotSDLPoint, SDL_FLIP_NONE);
        SDL_SetTextureColorMod(tex, 255, 255, 255);
        SDL_SetTextureAlphaMod(tex, 255);
    }
}

void Renderer::RenderImageRequest() {
    while (!ImageQue.empty()) {
        ImageRenderRequest imageRequest = ImageQue.front();
        ImageQue.pop();
        const int pixels_per_meter = 100;
        glm::vec2 final_rendering_position = glm::vec2(imageRequest.x, imageRequest.y) - cameraPosition;

        SDL_Texture* tex = GetImage(imageRequest.image);
        SDL_Rect tex_rect;
        SDL_QueryTexture(tex, NULL, NULL, &tex_rect.w, &tex_rect.h);

        // Apply scale
        SDL_RendererFlip flip_mode = SDL_FLIP_NONE;
        if (imageRequest.scale_x < 0) flip_mode = SDL_FLIP_HORIZONTAL;
        if (imageRequest.scale_y < 0) flip_mode = SDL_FLIP_VERTICAL;

        float x_scale = std::abs(imageRequest.scale_x);
        float y_scale = std::abs(imageRequest.scale_y);

        tex_rect.w *= x_scale;
        tex_rect.h *= y_scale;

        SDL_Point pivot_point = { static_cast<int>(imageRequest.pivot_x * tex_rect.w), static_cast<int>(imageRequest.pivot_y * tex_rect.h) };

        tex_rect.x = static_cast<int>(final_rendering_position.x * pixels_per_meter + cameraWidth * 0.5f * (1.0f / zoomFactor) - pivot_point.x);
        tex_rect.y = static_cast<int>(final_rendering_position.y * pixels_per_meter + cameraHeight * 0.5f * (1.0f / zoomFactor) - pivot_point.y);

        // Apply tint / alpha to texture
        SDL_SetTextureColorMod(tex, imageRequest.color.r, imageRequest.color.g, imageRequest.color.b);
        SDL_SetTextureAlphaMod(tex, imageRequest.color.a);

        // Perform draw
        Helper::SDL_RenderCopyEx498(-1, "", renderer, tex, NULL, &tex_rect, static_cast<int>(imageRequest.rotation), &pivot_point, static_cast<SDL_RendererFlip>(flip_mode));

        // Remove tint / alpha from texture
        SDL_SetTextureColorMod(tex, 255, 255, 255);
        SDL_SetTextureAlphaMod(tex, 255);
    }
}

void Renderer::RenderPixelRequest() {
    while (!PixelQue.empty()) {
        DrawPixelRequest pixelRequest = PixelQue.front();
        PixelQue.pop();
        SDL_SetRenderDrawColor(renderer, pixelRequest.color.r, pixelRequest.color.g, pixelRequest.color.b, pixelRequest.color.a);
        SDL_RenderDrawPoint(renderer, pixelRequest.x, pixelRequest.y);
    }
}



TTF_Font* Renderer::TextRenderRequest::LoadFont(std::string font, int size) {
    std::string directoryPath = "resources/fonts/";
    if (std::filesystem::exists(directoryPath)) {
        for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".ttf" && entry.path().filename().stem().string() == font) {
                TTF_Font* font = TTF_OpenFont(entry.path().string().c_str(), size);
                std::string fontName = entry.path().filename().stem().string();
                fonts[fontName][size] = font;
                return font;
            }
        }
    }
    std::cout << "error: font " << font << " missing";
    exit(0);
}

SDL_Texture* Renderer::GetImage(const std::string& name) {
    auto it = images.find(name);
    if (it != images.end()) {
        return it->second;
    }
    std::cout << "error: missing image " << name;
    exit(0);
    return nullptr;
}

void Renderer::LoadImagesFromDirectory(const std::string& directoryPath) {
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

void Renderer::SetPosition(float x, float y) {
    cameraPosition.x = x;
    cameraPosition.y = y;
}

float Renderer::GetPositionX() {
    return cameraPosition.x;
}

float Renderer::GetPositionY() {
    return cameraPosition.y;
}

void Renderer::SetZoom(float zoom_factor) {
    zoomFactor = zoom_factor;
}

float Renderer::GetZoom() {
    return zoomFactor;
}