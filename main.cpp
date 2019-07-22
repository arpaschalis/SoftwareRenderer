#include "stdlib.h"
#include "SdlWrapper.h"
#include "SDL.h"
#include "Geometry.h"
#include "Model.h"
#include "OurGL.h"
#include <algorithm>

SdlWrapper sdl = SdlWrapper(width, height);
Model* model;

Vector3f lightDirection(0.7f, 0.0f, 1.0f);
Vector3f eye(-5.0f, 3.0f, 15.0f);
Vector3f center(0.0f, 0.0f, 0.0f);
Vector3f up(0.0f, 1.0f, 0.0f);

struct GouraudDiffuseShader : public IShader {
	Vector3f vertLightIntensity;
	Vector2f uv_coords[3];

	virtual Vector3f vertex(int faceId, int whichVert) {
		Vector3f vert = model->getVertex(faceId, whichVert);
		vert = viewport * projection * modelView * vert;
		vertLightIntensity.raw[whichVert] = std::max(0.0f, model->getNormal(faceId, whichVert) * lightDirection);
		uv_coords[whichVert] = model->getUvCoords(faceId, whichVert);
		return vert;
	}

	virtual bool fragment(Vector3f barycentric, TGAColor &color) {
		float intensity = vertLightIntensity * barycentric;
		Vector2f uv =  uv_coords[0] * barycentric.x + uv_coords[1] * barycentric.y +  uv_coords[2] * barycentric.z;
		color = model->getDiffuseColor(uv) * intensity;
		return false;
	}
};


struct StripeShader : public IShader {
	Vector3f vertLightIntensity;
	Vector2f uv_coords[3];
	Vector3f vert_x;

	virtual Vector3f vertex(int faceId, int whichVert) {
		Vector3f vert = model->getVertex(faceId, whichVert);
		vert_x.raw[whichVert] = vert.x;
		vert = viewport * projection * modelView * vert;
		vertLightIntensity.raw[whichVert] = std::max(0.0f, model->getNormal(faceId, whichVert) * lightDirection);
		uv_coords[whichVert] = model->getUvCoords(faceId, whichVert);
		return vert;
	}

	virtual bool fragment(Vector3f barycentric, TGAColor &color) {
		float intensity = vertLightIntensity * barycentric;
		Vector2f uv = uv_coords[0] * barycentric.x + uv_coords[1] * barycentric.y + uv_coords[2] * barycentric.z;
		float x = vert_x * barycentric;
		color = model->getDiffuseColor(uv) * intensity;
		if (x > -0.3f && x < -0.2f)
			color.r = 100;

		return false;
	}
};


struct BlackWhiteShader : public IShader {
	Vector3f vertLightIntensity;
	Vector2f uv_coords[3];

	virtual Vector3f vertex(int faceId, int whichVert) {
		Vector3f vert = model->getVertex(faceId, whichVert);
		vert = viewport * projection * modelView * vert;
		vertLightIntensity.raw[whichVert] = std::max(0.0f, model->getNormal(faceId, whichVert) * lightDirection);
		uv_coords[whichVert] = model->getUvCoords(faceId, whichVert);
		return vert;
	}

	virtual bool fragment(Vector3f barycentric, TGAColor &color) {
		float intensity = vertLightIntensity * barycentric;
		Vector2f uv = uv_coords[0] * barycentric.x + uv_coords[1] * barycentric.y + uv_coords[2] * barycentric.z;
		color = model->getDiffuseColor(uv) * intensity;
		char value = std::max(color.r, std::max(color.g, color.b));
		color = TGAColor(value, value, value, 255);

		return false;
	}
};


int main(int argc, char* args[]) {
	// Begin.
	sdl.clearScreen();
	clearZBuffer();
	model = new Model("obj\\african_head.obj");

	lightDirection.normalize();
	setProjection(3.0f);
	setViewport(1, 1, (float)width, (float)height, 255.0f);
	lookAt(eye, center, up);

	BlackWhiteShader shader;
	Vector3f screen_coords[3];
	for (int i = 0; i < model->nfaces(); i++) {
		for (int j = 0; j < 3; j++) {
			screen_coords[j] = shader.vertex(i, j);
		}
		triangle(screen_coords, sdl, shader);
	}

	// End.
	sdl.present();

	// Wait for window close or ESC button.
	bool quit = false;
	SDL_Event e;
	while (!quit) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT)
				quit = true;
			if (e.type == SDL_KEYDOWN)
				if (e.key.keysym.sym == SDLK_ESCAPE)
					quit = true;
		}
	}

	return 0;
}