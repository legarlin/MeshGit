#pragma once

#include "glm\glm.hpp"
#include <maya/MPointArray.h>
#include <maya/MPoint.h>
#include <unordered_set>

using namespace glm;

class MeshComponent {

public:
    enum Type { VERTEX, FACE, EDGE, EMPTY } type;

    MPoint pos;
    MeshComponent();
    MeshComponent(Type t, MPoint position);
    bool isEqualTo(MeshComponent* other);

   // Adjacency Information
    std::unordered_set<MeshComponent*> adjacentVertices;
    std::unordered_set<MeshComponent*> adjacentFaces;
    std::unordered_set<MeshComponent*> adjacentComponents;

	void addAdjacency(MeshComponent* other);
};