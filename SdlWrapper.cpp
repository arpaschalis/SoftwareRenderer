#include "SdlWrapper.h"
#include <iostream>


SdlWrapper::SdlWrapper(int width, int height) {
	winHeight = height;

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cout << "SDL_Init_Error: " << SDL_GetError() << std::endl;
		return;
	}

	Uint32 flags = 0;
//	flags = SDL_WINDOW_RESIZABLE;

	// Make window
	win = SDL_CreateWindow("Software Renderer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		width, height, flags);
	if (win == NULL) {
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return;
	}

	// Prepare renderer
	renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL) {
		SDL_DestroyWindow(win);
		std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return;
	}

	outputImage.init(width, height, TGAImage::RGB);
}


SdlWrapper::~SdlWrapper() {
	outputImage.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	outputImage.write_tga_file("..\\output\\output.tga");
}


void SdlWrapper::clearScreen() {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
}


void SdlWrapper::drawPoint(int x, int y, TGAColor &color) {
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	SDL_RenderDrawPoint(renderer, x, winHeight - y);

	outputImage.set(x, y, color);
}


void SdlWrapper::present() {
	SDL_RenderPresent(renderer);
}