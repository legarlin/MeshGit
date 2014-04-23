#include "MeshComponent.h"

MeshComponent::MeshComponent(Type t, MPoint position, int i) {
	pos = position;
	type = t;
	index = i;
}

MeshComponent::MeshComponent() {
	type = EMPTY;
	//TODO Check if this is actually set
}


//TODO need to make this work for other enum types as well
bool MeshComponent::isEqualTo(MeshComponent* other){
	if(type == VERTEX &&  other->type == VERTEX){
		if(pos == other->pos){
			return true;
		}
	}
	else if (type == EMPTY && other->type == EMPTY){
		return true;
	}

	return false;
}

void MeshComponent::addAdjacency(MeshComponent* adjacentComponent, bool isSecondAdd){
	
	//We dont want to add adj info to itself
	if(this == adjacentComponent) 
		return;

	if(adjacentComponent->type == VERTEX){
		adjacentVertices.insert(adjacentComponent);
	}
	else 
		adjacentFaces.insert(adjacentComponent);

	adjacentComponents.insert(adjacentComponent);//contains all adjacent components

	if(!isSecondAdd)
		adjacentComponent->addAdjacency(this,true);

}

bool MeshComponent::hasAdjacency(MeshComponent* adjacentComponent){
	int num = this->adjacentComponents.count(adjacentComponent);
	if(num>0)
		return true;
	else 
		return false;
}