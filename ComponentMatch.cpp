#include "ComponentMatch.h"

ComponentMatch::ComponentMatch(MeshComponent &originalComponent, MeshComponent&derivComponent) { 
	originalComp = originalComponent;
	derivativeComp = derivComponent;
	
	
	//CALCULATE THE COST OF THIS MATCHING
	cost = 0.0;
	//If both are vertices just compute the geometric cost 
	if(originalComp.type==MeshComponent::VERTEX && originalComp.type==MeshComponent::VERTEX){
		addGeometricCost();
		
	}
	else if( (originalComp.type!=MeshComponent::EMPTY && derivComponent.type==MeshComponent::EMPTY) 
		|| (derivComponent.type!=MeshComponent::EMPTY && originalComp.type==MeshComponent::EMPTY)){
		//this is an unmatched pairing, e.g. vert with no match in the other geometry
		double costUnmatched = 1.0;
		cost = costUnmatched;
	}

}
ComponentMatch::ComponentMatch() { 
}

void ComponentMatch::addAdjacencyCost() { 
	double adjCost = 0.0;

	cost += adjCost; 
}

void ComponentMatch::addGeometricCost() {

	//Geometric cost = cost of position difference + cost of orientation diff(wrt normals of adjacent faces)

	//1-Cost of position difference = d/(d+1)
	MPoint vertA = originalComp.pos;
	MPoint vertB = derivativeComp.pos;
	double distance= vertA.distanceTo(vertB);
	double costP = distance/(distance+1);

	//2 - TODO Cost of orientation, need adjacent face data which we don't have at the moment 
	double costO=0.0;

	cost += costP + costO; 


}

Match ComponentMatch::getMatches() {
	Match match;
	match.originalComp = originalComp;
	match.derivativeComp = derivativeComp;

	return match;
}

double ComponentMatch::getCost() {
	return cost;
}

bool ComponentMatch::isEqualTo(ComponentMatch other){
	//TO DO - expand for faces case as well 
	if(originalComp.isEqualTo(other.originalComp) && 
		(derivativeComp.isEqualTo(other.derivativeComp))){
			return true;
	}
	return false;
}