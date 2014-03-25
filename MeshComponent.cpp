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
bool MeshComponent::isEqualTo(MeshComponent other){
	if(type == VERTEX &&  MeshComponent.type == VERTEX){
		if(pos == other.pos){
			return true;
		}
	}
	return false;
}