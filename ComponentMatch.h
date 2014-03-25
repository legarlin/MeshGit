#pragma once

#include "MeshComponent.h"

struct Match {
	MeshComponent originalComp;
	MeshComponent derivativeComp;
};

class ComponentMatch {
	MeshComponent originalComp;
	MeshComponent derivativeComp;

	double cost;

public:
	ComponentMatch(MeshComponent &a, MeshComponent&b);

	void addGeometricCost();
	void addAdjacencyCost();

	Match getMatches();

	double getCost();
};