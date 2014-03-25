#include "ComponentMatch.h"

ComponentMatch::ComponentMatch(MeshComponent &a, MeshComponent&b) { 
	originalComp = a;
	derivativeComp = b;

	if(originalComp.type==MeshComponent::Vertex && originalComp.type==MeshComponent::Vertex){

		
	}

}

void ComponentMatch::addAdjacencyCost() { 

	double geomCost = 0.0;


}

void ComponentMatch::addGeometricCost() { }

Match ComponentMatch::getMatches() {
	Match match;
	match.originalComp = originalComp;
	match.derivativeComp = derivativeComp;

	return match;
}

double ComponentMatch::getCost() {
	return cost;
}