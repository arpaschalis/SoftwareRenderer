#include "OurGL.h"
#include <algorithm>


Matrix modelView;
Matrix viewport;
Matrix projection;

const int width = 600;
const int height = 600;

int zBuffer[height][width];


void clearZBuffer() {
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			zBuffer[y][x] = std::numeric_limits<int>::min();
		}
	}
}

void setViewport(float x, float y, float w, float h, float depth) {
	viewport = Matrix::Identity();
	viewport.m[0][3] = x + w / 2.0f;
	viewport.m[1][3] = y + h / 2.0f;
	viewport.m[2][3] = depth / 2.0f;

	viewport.m[0][0] = w / 2.0f;
	viewport.m[1][1] = h / 2.0f;
	viewport.m[2][2] = depth / 2.0f;
}


void setProjection(float c) {
	projection = Matrix(1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, -1.0f / c, 1.0f);
}


void lookAt(Vector3f eye, Vector3f center, Vector3f up) {
	Vector3f z = (eye - center).normalize();
	Vector3f x = (up ^ z).normalize();
	Vector3f y = (z ^ x).normalize();
	Matrix mInv = Matrix::Identity();
	Matrix tr = Matrix::Identity();
	for (int i = 0; i < 3; i++) {
		mInv.m[0][i] = x.raw[i];
		mInv.m[1][i] = y.raw[i];
		mInv.m[2][i] = z.raw[i];
		tr.m[i][3] = -center.raw[i];
	}
	modelView = mInv * tr;
}


Vector3f barycentric(Vector3i t0, Vector3i t1, Vector3i t2, Vector2i p) {
	Vector3f vec1(float(t1.x - t0.x), float(t2.x - t0.x), float(t0.x - p.x));
	Vector3f vec2(float(t1.y - t0.y), float(t2.y - t0.y), float(t0.y - p.y));
	Vector3f u = vec1 ^ vec2;

	// t0, t1, t2, p have integer value as coordinates. So `abs(u[2])` < 1 means `u[2]` is 0.
	// That means triangle is degenerate, in this case return something with negative coordinates
	if (std::abs(u.z) < 1) return Vector3f(-1, 1, 1);

	return Vector3f(1.0f - (u.x + u.y) / u.z, u.x / u.z, u.y / u.z);
}


void triangle(Vector3f screen_coords[3], SdlWrapper &sdl, IShader &shader, bool toDepthBuffer) {
	// TODO: We previously accepted integer vectors for the vertex descriptions.
	// Try it again if there are accurancy problems.
	Vector3i vertex[3];
	vertex[0] = Vector3i((int)screen_coords[0].x, (int)screen_coords[0].y, (int)screen_coords[0].z);
	vertex[1] = Vector3i((int)screen_coords[1].x, (int)screen_coords[1].y, (int)screen_coords[1].z);
	vertex[2] = Vector3i((int)screen_coords[2].x, (int)screen_coords[2].y, (int)screen_coords[2].z);

	// Find bouncing Box
	int xMin = (int)std::min(vertex[0].x, std::min(vertex[1].x, vertex[2].x));
	int yMin = (int)std::min(vertex[0].y, std::min(vertex[1].y, vertex[2].y));
	int xMax = (int)std::max(vertex[0].x, std::max(vertex[1].x, vertex[2].x));
	int yMax = (int)std::max(vertex[0].y, std::max(vertex[1].y, vertex[2].y));

	TGAColor color;
	for (int y = yMin; y < yMax; y++) {
		for (int x = xMin; x < xMax; x++) {
			if (x < 0 || x >= width || y < 0 || y >= height)
				continue;

			Vector3f vecBarycentric = barycentric(vertex[0], vertex[1], vertex[2], Vector2i(x, y));
			if (vecBarycentric.x >= 0 && vecBarycentric.y >= 0 && vecBarycentric.z >= 0) {
				bool toDiscard = shader.fragment(vecBarycentric, color);
				int z = int(vecBarycentric.x * vertex[0].z + vecBarycentric.y * vertex[1].z + vecBarycentric.z * vertex[2].z);
				if (toDiscard == false && z > zBuffer[y][x]) {
					sdl.drawPoint(x, y, color, toDepthBuffer);
					zBuffer[y][x] = z;
				}
			}
		}
	}
}