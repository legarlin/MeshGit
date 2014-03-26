#pragma once

#include <vector>

#include "ComponentMatch.h"
#include "MeshComponent.h"
#include "glm\glm.hpp"
#include <maya/MPointArray.h>

using namespace std;
using namespace glm;

class MatchComputer {

	vector<MeshComponent> originalMeshComponents ; 
	vector<MeshComponent> derivativeMeshComponents;

	//Keeps track of how many unmatched elements there are intially
	int numUnmatched;

	vector<ComponentMatch> bestComponentMatches;
	vector<ComponentMatch> allComponentMatches; // all possible matches

	void makeHeap(vector<ComponentMatch> &data); // make heap, wrapper function
	void heapify(int index, vector<ComponentMatch> &data);
	void swap(int index1, int index2, vector<ComponentMatch> &data);

public:
	MatchComputer();
	MatchComputer(MPointArray &meshVerts1, MPointArray &meshVerts2);
	void makeComponentMatches();
	void makeComponents(MPointArray &meshVerts1, MPointArray &meshVerts2);
	void matchGreedy();
	int iterationCount;

	//Greedy algorithm helper methods
	double computeCost(vector<ComponentMatch> matches, int numUnmatched);
	bool areMatchComponentsInVector(ComponentMatch matchA, vector<ComponentMatch> matches);

	ComponentMatch getAndRemoveLowestComponentMatch();
};