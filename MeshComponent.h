#pragma once

#include "glm\glm.hpp"
#include <maya/MPointArray.h>
#include <maya/MPoint.h>

using namespace glm;

class MeshComponent {
	public MPoint pos;

public:
	enum Type { VERTEX, FACE, EDGE, EMPTY } type;

	MeshComponent();
	MeshComponent(MPoint vertexPos);
};