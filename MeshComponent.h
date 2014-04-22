#pragma once

#include "glm\glm.hpp"
#include <maya/MPointArray.h>
#include <maya/MPoint.h>
#include <unordered_set>
#include <set>

using namespace glm;

class MeshComponent {

public:
    enum Type { VERTEX, FACE, EDGE, EMPTY } type;

    MPoint pos;
    MeshComponent();
    MeshComponent(Type t, MPoint position);
    bool isEqualTo(MeshComponent* other);

   // Adjacency Information
    std::set<MeshComponent*> adjacentVertices;
    std::set<MeshComponent*> adjacentFaces;
    std::set<MeshComponent*> adjacentComponents;

	void addAdjacency(MeshComponent* other , bool isSecondAdd);
	bool hasAdjacency(MeshComponent* adjacentComponent);
};