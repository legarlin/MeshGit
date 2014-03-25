#pragma once

#include "MeshComponent.h"

struct Match {
	MeshComponent thing1;
	MeshComponent thing2;
};

class ComponentMatch {
	MeshComponent thing1;
	MeshComponent thing2;

	float cost;

public:
	ComponentMatch();

	void computeGeometricCost();
	void computeAdjacencyCost();

	Match getMatches();

	float getCost();
};