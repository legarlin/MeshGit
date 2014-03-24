#include "MeshComponent.h"

MeshComponent::MeshComponent() {
}

MeshComponent::MeshComponent(vec3 vertexPos) {
	pos = vertexPos;
	type = Vertex;
}
