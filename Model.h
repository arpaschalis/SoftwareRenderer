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
		return diffuse.get((int)(uv.u * diffuse.get_width()),
						   (int)(uv.y * diffuse.get_height()));
	};

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

	TGAImage diffuse;
};

#endif //__MODEL_H__