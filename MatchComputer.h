#pragma once

#include <vector>

#include "ComponentMatch.h"
#include "MeshComponent.h"
#include "glm\glm.hpp"

using namespace std;
using namespace glm;

class MatchComputer {

	vector<MeshComponent> thing1things; // lol awful names.
	vector<MeshComponent> thing2things;

	float totalCost;
	vector<ComponentMatch> lowestComponentMatches;
	vector<ComponentMatch> allComponentMatches; // all possible matches

public:
	MatchComputer();
	MatchComputer(vector<vec3> meshVerts1, vector<vec3> meshVerts2);

	void matchGreedy();
};