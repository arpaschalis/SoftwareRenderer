#include "stdlib.h"
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
		float shadow = 0.3f + 0.7f * (sdl.shadow_buffer.get(shadow_point.x, shadow_point.y).raw[2] < shadow_point.z + 43.34f);
		Vector2f uv =  uv_coords[0] * barycentric.x + uv_coords[1] * barycentric.y +  uv_coords[2] * barycentric.z;
		Vector3f normal = (projmodelview_IT * model->getNormal(uv)).normalize();
		Vector3f light = (projmodelview * lightDirection).normalize();
		Vector3f reflection = (normal * (normal * light * 2.0f) - light);
		float specular = pow(std::max(reflection.z, 0.0f), model->getSpecular(uv));
		float diffuse = std::max(0.0f, normal * light);
		color = model->getDiffuseColor(uv);
		for (int i = 0; i < 3; i++)
			color.raw[i] = (unsigned char)std::min<float>(0.0f + color.raw[i] * shadow *
					(diffuse + 0.6f * specular), 255.0f);
		if (shadow < 1.0f)
			shadow = 1.0f;

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
	setViewport(width / 8.0f, height / 8.0f, width * 3.0f / 4.0f, height * 3.0f / 4.0f, 255.0f);
	lookAt(lightDirection, center, up);
	DepthShader depthShader;
	Vector3f screen_coords[3];
	for (int i = 0; i < model->nfaces(); i++) {
		for (int j = 0; j < 3; j++) {
			screen_coords[j] = depthShader.vertex(i, j);
		}
		triangle(screen_coords, sdl, depthShader, true);
	}

	Matrix M = viewport * projection * modelView;

	clearZBuffer();
	setProjection(3.0f);
	setViewport(width / 8.0f, height / 8.0f, width * 3.0f / 4.0f, height * 3.0f / 4.0f, 255.0f);
	lookAt(eye, center, up);
	Shader shader;
	shader.projmodelview = projection * modelView;
	shader.projmodelview_IT = shader.projmodelview.invert().transpose();
	shader.shadow_transform = M * (viewport * projection * modelView).invert();
	for (int i = 0; i < model->nfaces(); i++) {
		for (int j = 0; j < 3; j++) {
			screen_coords[j] = shader.vertex(i, j);
		}
		triangle(screen_coords, sdl, shader, false);
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