#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"

Model::Model(const char *filename) : verts(), uvCoords(), normals() {
	std::ifstream in;
	in.open(filename, std::ifstream::in);
	if (in.fail()) return;
	std::string line;
	while (!in.eof()) {
		std::getline(in, line);
		std::istringstream iss(line.c_str());
		char trash;
		if (!line.compare(0, 2, "v ")) {
			iss >> trash;
			Vector3f v;
			for (int i = 0; i < 3; i++) iss >> v.raw[i];
			verts.push_back(v);
		} else if (!line.compare(0, 3, "vt ")) {
			Vector2f tex;
			iss >> trash >> trash >> tex.u >> tex.v >> trash;
			uvCoords.push_back(tex);
		} else if (!line.compare(0, 3, "vn ")) {
			iss >> trash >> trash;
			Vector3f v;
			for (int i = 0; i < 3; i++) iss >> v.raw[i];
			normals.push_back(v);
		} else if (!line.compare(0, 2, "f ")) {
			std::vector<int> v;
			std::vector<int> uv;
			std::vector<int> n;
			int vertexId, uvId, normalId;
			iss >> trash;
			while (iss >> vertexId >> trash >> uvId >> trash >> normalId) {
				vertexId--; // in wavefront obj all indices start at 1, not zero
				uvId--;
				normalId--;
				v.push_back(vertexId);
				uv.push_back(uvId);
				n.push_back(normalId);
			}
			Face face = { v, uv, n };
			faces.push_back(face);
		}
	}
	std::cerr << "# v# " << verts.size() << " vt# " << uvCoords.size() << " vn# "
		<< normals.size() << " f# " << faces.size() << std::endl;

	diffuseMap.read_tga_file("obj\\african_head_diffuse.tga");
	diffuseMap.flip_vertically();
	normalMap.read_tga_file("obj\\african_head_nm.tga");
	normalMap.flip_vertically();
	specularMap.read_tga_file("obj\\african_head_spec.tga");
	specularMap.flip_vertically();
}

Model::~Model() {
}

int Model::nverts() {
	return (int)verts.size();
}

int Model::nUvCoords() {
	return (int)uvCoords.size();
}


int Model::nNormals() {
	return (int)normals.size();
}

int Model::nfaces() {
	return (int)faces.size();
}
