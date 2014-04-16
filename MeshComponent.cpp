#include "MeshComponent.h"

MeshComponent::MeshComponent(MPoint vertexPos) {
	pos = vertexPos;
	type = VERTEX;
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