#include "MeshComponent.h"

MeshComponent::MeshComponent() {
}

MeshComponent::MeshComponent(MPoint vertexPos) {
	pos = vertexPos;
	type = VERTEX;
}

MeshComponent::MeshComponent() {
	type = EMPTY;
}
