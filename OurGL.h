#pragma once

#include "Geometry.h"
#include "SdlWrapper.h"

extern Matrix modelView;
extern Matrix viewport;
extern Matrix projection;

extern const int width;
extern const int height;

void clearZBuffer();
void setViewport(float x, float y, float w, float h, float depth);
void setProjection(float c);
void lookAt(Vector3f eye, Vector3f center, Vector3f up);

struct IShader {
	virtual Vector3f vertex(int faceId, int vertId) = 0;
	virtual bool fragment(Vector3f barycentric, TGAColor &color) = 0;
};

void triangle(Vector3f vertex[3], SdlWrapper &sdl, IShader &shader, bool toDepthBuffer);
