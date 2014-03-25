#pragma once

#include <vector>

#include "ComponentMatch.h"
#include "MeshComponent.h"
#include "glm\glm.hpp"
#include <maya/MPointArray.h>

using namespace std;
using namespace glm;

class MatchComputer {

	vector<MeshComponent> originalMeshComponents; // lol awful names.
	vector<MeshComponent> derivativeMeshComponents;

	float totalCost;
	vector<ComponentMatch> lowestComponentMatches;
	vector<ComponentMatch> allComponentMatches; // all possible matches

	void makeHeap(vector<ComponentMatch> &data); // make heap, wrapper function
	void heapify(int index, vector<ComponentMatch> &data);
	void swap(int index1, int index2, vector<ComponentMatch> &data);

public:
	MatchComputer();
	MatchComputer(MPointArray &meshVerts1, MPointArray &meshVerts2);

	void matchGreedy();
};