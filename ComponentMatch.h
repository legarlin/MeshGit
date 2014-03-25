#pragma once

#include "MeshComponent.h"

struct Match {
	MeshComponent originalComp;
	MeshComponent derivativeComp;
};

class ComponentMatch {


	double cost;

public:
	ComponentMatch(MeshComponent &a, MeshComponent&b);
	ComponentMatch();
	MeshComponent originalComp;
	MeshComponent derivativeComp;

	void addGeometricCost();
	void addAdjacencyCost();
	bool isEqualTo(ComponentMatch other);
	Match getMatches();

	double getCost();
};