#include <sstream>

#include "OBJLoader.hpp"

bool loadOBJ(const std::string& path, std::vector<Vertex> &vertices, std::vector<uint32_t> &indices)
{
	std::vector<glm::vec3> verticesPositions;
	//std::vector<glm::vec2> verticesTexturesCoords;
	std::vector<glm::vec3> verticesNormals;

	//std::vector<GLint> vertexTextureCoordIndicies;
	std::vector<GLint> vertexNormalIndicies;

	std::stringstream ss;
	std::ifstream inFile(path);
	std::string line;

	if (inFile.is_open() == false) {
		std::cout << "Could not open " << path << std::endl;
		return false;
	}

	while (std::getline(inFile, line)) {
		std::string prefix;

		ss.clear();
		ss.str(line);
		ss >> prefix;

		if (prefix == "v") {
			glm::vec3 vertex;
			ss >> vertex.x >> vertex.y >> vertex.z;
			verticesPositions.push_back(vertex);
		}/*
		else if (prefix == "vt") {
			glm::vec2 textureCoord;
			ss >> textureCoord.x >> textureCoord.y;
			verticesTexturesCoords.push_back(textureCoord);
		}*/
		else if (prefix == "vn") {
			glm::vec3 normal;
			ss >> normal.x  >> normal.y >> normal.z;
			verticesNormals.push_back(normal);
		}
		else if (prefix == "f") {
			GLint tempInt = 0;
			int counter = 0;
			int indicesNbr = 0;
			while (ss >> tempInt) {
				if (counter == 0) {
					indices.push_back(tempInt - 1);
					if (indicesNbr == 3) {
						indices.push_back(indices[indices.size() - 4]);
						indices.push_back(indices[indices.size() - 3]);
					}
					indicesNbr++;
				}
				else if (counter == 1) {
					//vertexTextureCoordIndicies.push_back(tempInt);
				}
				else if (counter == 2) {
					vertexNormalIndicies.push_back(tempInt);
				}

				if (ss.peek() == '/') {
					counter++;
					ss.ignore(1, '/');
				}
				else if (ss.peek() == ' ') {
					counter++;
					ss.ignore(1, ' ');
				}

				if (counter > 2) {
					counter = 0;
				}
			}
		}
	}
	vertices.resize(verticesPositions.size(), Vertex());
	//texturesCoords.resize(vertexTextureCoordIndicies.size(), glm::vec2());
	for (size_t i = 0; i < vertices.size(); i++) {
		vertices[i].position = verticesPositions[i];
		vertices[i].color = glm::vec3(1.0f, 1.0f, 1.0f);
		vertices[i].normal = verticesNormals[i];
		//normals[i] = verticesNormals[vertexNormalIndicies[i] - 1];
		//texturesCoords[i] = verticesTexturesCoords[vertexTextureCoordIndicies[i] - 1];
	}

	std::cout << "Loaded " << indices.size() << " indices" << std::endl;
	std::cout << "Loaded " << vertices.size() << " vertices" << std::endl;

	return true;
}
