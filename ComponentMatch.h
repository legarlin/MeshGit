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
	ComponentMatch();

	void addGeometricCost();
	void addAdjacencyCost();
	bool isEqualTo(ComponentMatch other);
	Match getMatches();

	double getCost();
};