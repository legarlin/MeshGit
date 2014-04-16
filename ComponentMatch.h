#pragma once

#include "MeshComponent.h"

struct Match {
	MeshComponent* originalComp;
	MeshComponent* derivativeComp;
};

struct Color {
	float r;
	float g;
	float b;
};

class ComponentMatch {

	double cost;

public:
	ComponentMatch(MeshComponent* a, MeshComponent* b);
	ComponentMatch();
	MeshComponent* originalComp;
	MeshComponent* derivativeComp;

	void addGeometricCost();
	void addAdjacencyCost();
	bool isEqualTo(ComponentMatch* other);
	Match getMatches();

	double getCost();

	Color color;
};