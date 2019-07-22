#ifndef __SDL_H__
#define __SDL_H__

#include "SDL.h"
#include "tgaimage.h"

class SdlWrapper {
public:
	SdlWrapper(int width, int height);
	~SdlWrapper();

	void clearScreen();
	void drawPoint(int x, int y, TGAColor &color);
	void present();

private:
	SDL_Renderer*	renderer;
	SDL_Window*		win;
	int				winHeight;	// used to flip image;

	TGAImage outputImage;
};

#endif //__SDL_H__