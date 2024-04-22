#ifndef RENDERER_H
#define RENDERER_H

#include <string>
#include <queue>
#include <unordered_map>
#include <iostream>
#include <filesystem>
#include "Helper.h"
#include "../SDL2/SDL.h"
#include "../SDL2_image/SDL_image.h"
#include "../SDL2_ttf/SDL_ttf.h"
#include "../SDL2_mixer/SDL_mixer.h"
#include "../Third Party/glm/glm/glm.hpp"

class Renderer {
public:
	class TextRenderRequest
	{
	public:
		std::string text;
		std::string font;
		SDL_Color color;
		int size;
		int x;
		int y;

		TextRenderRequest();
		TextRenderRequest(
			std::string text, std::string font, SDL_Color color, int size, int x, int y);

		static TTF_Font* LoadFont(std::string font, int size);

		static inline std::unordered_map<std::string, std::unordered_map<int, TTF_Font*>> fonts;
	private:
	};

	class ImageRenderRequest
	{
	public:
		std::string image;
		SDL_Color color;
		float x;
		float y;
		int sorting_order;
		float rotation;
		float scale_x;
		float scale_y;
		float pivot_x;
		float pivot_y;

		ImageRenderRequest();
		ImageRenderRequest(std::string image, SDL_Color color, float x, float y, int sorting_order, float rotation, float scale_x, float scale_y, float pivot_x, float pivot_y);
	};

	class UIRenderRequest
	{
	public:
		std::string image;
		SDL_Color color;
		int x;
		int y;
		int sorting_order;

		UIRenderRequest();
		UIRenderRequest(std::string image, SDL_Color color, float x, float y, int sorting_order);
	};

	class DrawPixelRequest
	{
	public:
		SDL_Color color;
		int x;
		int y;

		DrawPixelRequest();
		DrawPixelRequest(SDL_Color color, float x, float y);
	};

	Renderer(SDL_Renderer* renderer, int cameraWidth, int cameraHeight);

	SDL_Renderer* renderer;

	int cameraWidth;

	int cameraHeight;

	static void TextDraw(std::string text, float x, float y, std::string font, float size, float r, float g, float b, float a);

	void RenderTextRequest();

	void RenderUIRequest();

	void RenderImageRequest();

	void RenderPixelRequest();

	void RenderRequests();

	static void UIDraw(std::string image_name, float x, float y);

	static void UIDrawEx(std::string image_name, float x, float y, float r, float g, float b, float a, int sorting_order);

	static void ImageDraw(std::string image_name, float x, float y);

	static void ImageDrawEX(std::string image_name, float x, float y, float rotation, float scale_x, float scale_y, float pivot_x, float pivot_y, float r, float g, float b, float a, int sorting_order);

	static void PixelDraw(float x, float y, float r, float g, float b, float a);

	static void SetPosition(float x, float y);

	static float GetPositionX();

	static float GetPositionY();

	static void SetZoom(float zoom_factor);

	static float GetZoom();

	SDL_Texture* GetImage(const std::string& name);

	void LoadImagesFromDirectory(const std::string& directoryPath);

	static inline float zoomFactor;

	static inline glm::vec2 cameraPosition;
private:
	static inline std::queue<TextRenderRequest> TextQue;

	static inline std::queue<ImageRenderRequest> ImageQue;

	static inline std::queue<DrawPixelRequest> PixelQue;

	static inline std::queue<UIRenderRequest> UIQue;

	static inline std::unordered_map<std::string, SDL_Texture*> images;
};

#endif // RENDERER_H

