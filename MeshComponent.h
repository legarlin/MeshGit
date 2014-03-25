#pragma once

#include "glm\glm.hpp"
#include <maya/MPointArray.h>
#include <maya/MPoint.h>

using namespace glm;

class MeshComponent {
	MPoint pos;

public:
	enum Type { Vertex, Face, Edge } type;

	MeshComponent();
	MeshComponent(MPoint vertexPos);
};