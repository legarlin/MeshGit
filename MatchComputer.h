#pragma once

#include <vector>

#include "ComponentMatch.h"
#include "MeshComponent.h"

using namespace std;

class MatchComputer {

	float totalCost;
	vector<ComponentMatch> lowestComponentMatches;
	vector<ComponentMatch> allComponentMatches; // all possible matches

public:
	MatchComputer();

	void matchGreedy();
};