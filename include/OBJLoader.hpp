#ifndef OBJLOADER_HPP
#define OBJLOADER_HPP

#include <GL/glew.h>

#pragma warning(push, 0)
#include <glm/glm.hpp>
#pragma warning(pop, 0)

#include <vector>
#include <string>

#include "Vertex.hpp"

bool loadOBJ(const std::string& path, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);

#endif // !OBJLOADER_HPP