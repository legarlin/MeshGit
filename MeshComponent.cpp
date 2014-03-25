#include "MeshComponent.h"

MeshComponent::MeshComponent(MPoint vertexPos) {
	pos = vertexPos;
	type = VERTEX;
}

MeshComponent::MeshComponent() {
	type = EMPTY;
	//TODO Check if this is actually set
}
