#pragma once

#include <maya/MPointArray.h>
#include <maya/MPoint.h>
#include <unordered_set>
#include <set>
#include <vector>

#include "ComponentMatch.h"

class ComponentMatch;

using namespace std;

class MeshComponent {

public:
    enum Type { VERTEX, FACE, EDGE, EMPTY } type;

	int index;
    MPoint pos;

	// Adjacency Information
    set<MeshComponent*> adjacentVertices;
    set<MeshComponent*> adjacentFaces;
    set<MeshComponent*> adjacentComponents;

	ComponentMatch* matched;

    MeshComponent();
    MeshComponent(Type t, MPoint position, int i);

    bool isEqualTo(MeshComponent* other);

	void addAdjacency(MeshComponent* other , bool isSecondAdd);
	bool hasAdjacency(MeshComponent* adjacentComponent);

	void addComponentMatch(ComponentMatch*cM);
};