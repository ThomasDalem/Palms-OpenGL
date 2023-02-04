#include <sstream>
#include <unordered_map>
#include <chrono>

#include "OBJLoader.hpp"

struct vInfos {
	uint32_t vertexIndex;
	uint32_t textureCoordIndex;
	uint32_t normalIndex;
};

long findVertexIndex(vInfos& vertex, std::vector<vInfos>& vertices)
{
	for (int i = 0; i < vertices.size(); i++) {
		if (vertices[i].vertexIndex == vertex.vertexIndex && vertices[i].normalIndex == vertex.normalIndex) {
			return i;
		}
	}
	return -1;
}

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

	std::vector<vInfos> trios;

	auto start = std::chrono::high_resolution_clock::now();

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
			vInfos vertex;
			while (ss >> tempInt) {
				if (counter == 0) {
					vertex.vertexIndex = tempInt - 1;
				}
				else if (counter == 1) {
					vertex.textureCoordIndex = tempInt - 1;
					//vertexTextureCoordIndicies.push_back(tempInt);
				}
				else if (counter == 2) {
					vertex.normalIndex = tempInt - 1;
					//vertexNormalIndicies.push_back(tempInt);
				}

				if (ss.peek() == '/') {
					counter++;
					ss.ignore(1, '/');
				}
				else if (ss.peek() == ' ') {
					counter++;
					ss.ignore(1, ' ');
				}
				else if (ss.eof()) {
					counter++;
				}

				if (counter > 2) {
					long index = findVertexIndex(vertex, trios);
					if (index == -1) {
						trios.push_back(vertex);
						Vertex v;
						v.position = verticesPositions[vertex.vertexIndex];
						v.color = glm::vec3(0.f, 0.5f, 0.f);
						v.normal = verticesNormals[vertex.normalIndex];
						vertices.push_back(v);
						indices.push_back(static_cast<uint32_t>(vertices.size() - 1));
					}
					else {
						indices.push_back(index);
					}
					if (indicesNbr == 3) {
						indices.push_back(indices[indices.size() - 4]);
						indices.push_back(indices[indices.size() - 3]);
					}
					indicesNbr++;
					counter = 0;
				}
			}
		}
	}
	auto end = std::chrono::high_resolution_clock::now();

	auto duration = duration_cast<std::chrono::microseconds>(end - start);
	std::cout << "Duration: " << duration.count() << std::endl;

	std::cout << "Loaded " << indices.size() << " indices" << std::endl;
	std::cout << "Loaded " << vertices.size() << " vertices" << std::endl;

	return true;
}
