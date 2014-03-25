#include "ComponentMatch.h"

ComponentMatch::ComponentMatch() { }

void ComponentMatch::computeAdjacencyCost() { }

void ComponentMatch::computeGeometricCost() { }

Match ComponentMatch::getMatches() {
	Match match;
	match.originalComp = originalComp;
	match.derivativeComp = derivativeComp;

	return match;
}

float ComponentMatch::getCost() {
	return cost;
}