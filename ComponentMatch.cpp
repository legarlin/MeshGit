#include "ComponentMatch.h"

ComponentMatch::ComponentMatch() { }

void ComponentMatch::computeAdjacencyCost() { }

void ComponentMatch::computeGeometricCost() { }

Match ComponentMatch::getMatches() {
	Match match;
	match.thing1 = thing1;
	match.thing2 = thing2;

	return match;
}