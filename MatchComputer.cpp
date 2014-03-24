#include "MatchComputer.h"

#include <queue>

MatchComputer::MatchComputer() { }

MatchComputer::MatchComputer(vector<vec3> meshVerts1, vector<vec3> meshVerts2) {

	for (int i = 0; i < meshVerts1.size(); i++) {
		MeshComponent component(meshVerts1[i]);
		component.type = MeshComponent::Vertex;
		thing1things.push_back(component);
	}

	for (int i = 0; i < meshVerts2.size(); i++) {
		MeshComponent component(meshVerts2[i]);
		component.type = MeshComponent::Vertex;
		thing2things.push_back(component);
	}
}

void MatchComputer::matchGreedy() {
	// make min heap but associate it to cost	
}