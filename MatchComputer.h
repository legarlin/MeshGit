#pragma once

#include <vector>

#include "ComponentMatch.h"
#include "MeshComponent.h"
#include "glm\glm.hpp"
#include <maya/MPointArray.h>

using namespace std;
using namespace glm;

class MatchComputer {

	vector<MeshComponent*> originalMeshComponents; 
	vector<MeshComponent*> derivativeMeshComponents;

	//Keeps track of how many unmatched elements there are intially
	int numUnmatched;

	//HEAP STUFF
	void makeHeap(vector<ComponentMatch*> &data); // make heap, wrapper function
	void heapify(int index, vector<ComponentMatch*> &data);
	void swap(int a, int b, vector<ComponentMatch*> &data);

public:
	MatchComputer();
	MatchComputer(MPointArray* originalVerts, MPointArray* derivativeVerts);
	void makeComponentMatches();
	void makeComponents(MPointArray* originalVerts, MPointArray* derivativeVerts);
	void matchGreedy();
	int iterationCount;

	//Greedy algorithm helper methods
	double computeCost(vector<ComponentMatch*> matches, int numUnmatched);
	bool areMatchComponentsInVector(ComponentMatch* matchA, vector<ComponentMatch*> matches);
	void removeFromUnmatched(ComponentMatch* match);
	ComponentMatch* getAndRemoveLowestComponentMatch();

	//Gredy Algorithm Data Structure
	vector<ComponentMatch*> allComponentMatches; // all possible matches
	vector<ComponentMatch*> bestComponentMatches;
	MPointArray* unmatchedOriginalMeshPoints;//<--used for drawing later
	MPointArray* unmatchedDerivativeMeshPoints;
};