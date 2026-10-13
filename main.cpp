#include "SdlWrapper.h"
#include "SDL.h"
#include "Geometry.h"
#include "Model.h"
#include "OurGL.h"
#include <algorithm>

SdlWrapper sdl = SdlWrapper(width, height);
Model* model;

Vector3f lightDirection(-1.4f, 1.0f, 2.0f);
Vector3f eye(5.0f, 3.0f, 15.0f);
Vector3f center(0.0f, 0.0f, 0.0f);
Vector3f up(0.0f, 1.0f, 0.0f);

const float cameraDistance = 3.0f;	// strength of the perspective effect
const float depthRange = 255.0f;	// z values on screen go from 0 to this
const float shadowBrightness = 0.3f;	// brightness of a pixel in shadow (1.0 = lit)
const float specularWeight = 0.6f;	// strength of the shiny highlights
// A pixel only counts as shadowed if something is closer to the light by more
// than this. Prevents surfaces from shadowing themselves. Value from the tutorial.
const float shadowBias = 43.34f;



struct DepthShader : public IShader {
	Vector3f vertex_coords[3];

	virtual Vector3f vertex(int faceId, int whichVert) {
		Vector3f vert = model->getVertex(faceId, whichVert);
		vert = viewport * projection * modelView * vert;
		vertex_coords[whichVert] = vert;
		return vert;
	}

	virtual bool fragment(Vector3f barycentric, TGAColor &color) {
		Vector3f point_coords = vertex_coords[0] * barycentric.x + vertex_coords[1] * barycentric.y +
				vertex_coords[2] * barycentric.z;
		color = TGAColor(255, 255, 255, 255) * (point_coords.z / 255.0f);
		return false;
	}
};


struct Shader : public IShader {
	Vector3f	vertex_coords[3];
	Vector2f	uv_coords[3];
	Matrix		projmodelview;
	Matrix		projmodelview_IT;	// inverted and transposed.
	Matrix		shadow_transform;

	virtual Vector3f vertex(int faceId, int whichVert) {
		Vector3f vert = model->getVertex(faceId, whichVert);
		vert = viewport * projection * modelView * vert;
		vertex_coords[whichVert] = vert;
		uv_coords[whichVert] = model->getUvCoords(faceId, whichVert);
		return vert;
	}

	virtual bool fragment(Vector3f barycentric, TGAColor &color) {
		Vector3f point_coords = vertex_coords[0] * barycentric.x + vertex_coords[1] * barycentric.y +
				vertex_coords[2] * barycentric.z;
		Vector3f shadow_point = shadow_transform * point_coords;
		bool lit = sdl.shadow_buffer.get(shadow_point.x, shadow_point.y).raw[2] < shadow_point.z + shadowBias;
		float shadow = lit ? 1.0f : shadowBrightness;
		Vector2f uv =  uv_coords[0] * barycentric.x + uv_coords[1] * barycentric.y +  uv_coords[2] * barycentric.z;
		Vector3f normal = (projmodelview_IT * model->getNormal(uv)).normalize();
		Vector3f light = (projmodelview * lightDirection).normalize();
		Vector3f reflection = (normal * (normal * light * 2.0f) - light);
		float specular = pow(std::max(reflection.z, 0.0f), model->getSpecular(uv));
		float diffuse = std::max(0.0f, normal * light);
		color = model->getDiffuseColor(uv);
		for (int i = 0; i < 3; i++)
			color.raw[i] = (unsigned char)std::min<float>(0.0f + color.raw[i] * shadow *
					(diffuse + specularWeight * specular), 255.0f);

		return false;
	}
};


void drawModel(IShader &shader, bool toShadowBuffer) {
	Vector3f screen_coords[3];
	for (int i = 0; i < model->numFaces(); i++) {
		for (int j = 0; j < 3; j++) {
			screen_coords[j] = shader.vertex(i, j);
		}
		triangle(screen_coords, sdl, shader, toShadowBuffer);
	}
}


int main(int argc, char* args[]) {
	// Begin.
	sdl.clearScreen();
	clearZBuffer();
	model = new Model("assets/head.obj");

	lightDirection.normalize();

	setProjection(cameraDistance);
	setViewport(width / 8.0f, height / 8.0f, width * 3.0f / 4.0f, height * 3.0f / 4.0f, depthRange);

	// Pass 1: draw the model as seen from the light into the shadow buffer.
	lookAt(lightDirection, center, up);
	DepthShader depthShader;
	drawModel(depthShader, true);

	Matrix M = viewport * projection * modelView;	// model -> screen, as seen from the light

	// Pass 2: draw the model as seen from the camera, using the shadow buffer.
	clearZBuffer();
	lookAt(eye, center, up);
	Shader shader;
	shader.projmodelview = projection * modelView;
	shader.projmodelview_IT = shader.projmodelview.invert().transpose();
	shader.shadow_transform = M * (viewport * projection * modelView).invert();
	drawModel(shader, false);

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
