#pragma once

#include "glm\glm.hpp"

using namespace glm;

class MeshComponent {
	vec3 pos;

public:
	enum Type { Vertex, Face, Edge } type;

	MeshComponent();
	MeshComponent(vec3 vertexPos);
};