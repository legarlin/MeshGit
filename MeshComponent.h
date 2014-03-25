#pragma once

#include "glm\glm.hpp"
#include <maya/MPointArray.h>
#include <maya/MPoint.h>

using namespace glm;

class MeshComponent {
	

public:
	enum Type { VERTEX, FACE, EDGE, EMPTY } type;
	MPoint pos;
	MeshComponent();
	MeshComponent(MPoint vertexPos);

};