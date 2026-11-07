#pragma once

#include <cstdint>
#include <glm/glm.hpp>

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float float32;
typedef double float64;

struct VertexPos {
	glm::vec3 position;
};

struct VertexPosCol {
	glm::vec3 position;
	glm::vec4 color;
};

struct VertexPosTex {
	glm::vec3 position;
	glm::vec2 textureCoord;
};

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 tangent;
	glm::vec2 textureCoord;
};
