#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "geometry.h"
#include "tgaimage.h"

class Model {
public:
	Model(const char *filename);
	~Model();
	int nverts();
	int nUvCoords();
	int nNormals();
	int nfaces();

	inline Vector3f getVertex(int faceId, int whichVert) {
		return verts[faces[faceId].vertexIds[whichVert]];
	};
	inline Vector2f getUvCoords(int faceId, int whichVert) {
		return uvCoords[faces[faceId].uvIds[whichVert]];
	};
	inline Vector3f getNormal(int faceId, int whichVert) {
		return normals[faces[faceId].normalIds[whichVert]];
	};

	inline TGAColor getDiffuseColor(Vector2f uv) {
		return diffuseMap.get((int)(uv.u * diffuseMap.get_width()),
						   (int)(uv.y * diffuseMap.get_height()));
	};

	inline Vector3f getNormal(Vector2f uv) {
		TGAColor color = normalMap.get((int)(uv.u * normalMap.get_width()),
			(int)(uv.v * normalMap.get_height()));
		Vector3f result;
		result.x = (float)color.r / 255.0f * 2.0f - 1.0f;
		result.y = (float)color.g / 255.0f * 2.0f - 1.0f;
		result.z = (float)color.b / 255.0f * 2.0f - 1.0f;
		return result;
	}

	inline float getSpecular(Vector2f uv) {
		return (float)specularMap.get((int)(uv.u * specularMap.get_width()),
			(int)(uv.y * specularMap.get_height())).raw[0];
	}

private:
	std::vector<Vector3f> verts;
	std::vector<Vector2f> uvCoords;
	std::vector<Vector3f> normals;

	struct Face {
		std::vector<int> vertexIds;
		std::vector<int> uvIds;
		std::vector<int> normalIds;
	};
	std::vector<Face> faces;

	TGAImage diffuseMap;
	TGAImage normalMap;
	TGAImage specularMap;
};

#endif //__MODEL_H__