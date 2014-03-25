#pragma once

#include "MeshComponent.h"

struct Match {
	MeshComponent originalComp;
	MeshComponent derivativeComp;
};

class ComponentMatch {
	MeshComponent originalComp;
	MeshComponent derivativeComp;

	float cost;

public:
	ComponentMatch();

	void computeGeometricCost();
	void computeAdjacencyCost();

	Match getMatches();

	float getCost();
};