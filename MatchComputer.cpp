#include "MatchComputer.h"

#include <queue>

MatchComputer::MatchComputer() { }

MatchComputer::MatchComputer(MPointArray &meshVerts1, MPointArray &meshVerts2) {

	//Convert into components and store them 
	for (int i = 0; i < meshVerts1.length(); i++) {
		MeshComponent component(meshVerts1[i]);
		component.type = MeshComponent::Vertex;
		originalMeshComponents.push_back(component);
	}

	for (int i = 0; i < meshVerts2.length(); i++) {
		MeshComponent component(meshVerts2[i]);
		component.type = MeshComponent::Vertex;
		derivativeMeshComponents.push_back(component);
	}

	//originalMeshComponents=meshVerts1;
	//derivativeMeshComponents=meshVerts2;
}

void MatchComputer::makeHeap(vector<ComponentMatch> &data) {
	int index = (int) data.size() / 2 - 1;

	for (int i = index; i >= 0; i--) {
		heapify(i, data);
	}
}

void MatchComputer::heapify(int index, vector<ComponentMatch> &data) {
	int leftIndex = index*2 + 1; // left
	int rightIndex = index*2 + 2; // right

	if (leftIndex < data.size()) {
		ComponentMatch curr = data[index];
		ComponentMatch left = data[leftIndex];

		if (rightIndex < data.size()) { // check if right child node exists
			ComponentMatch right = data[rightIndex];

			if (left.getCost() < right.getCost()) {
				if (left.getCost() < curr.getCost()) {
					swap(index, leftIndex, data);
					heapify(leftIndex, data);
				}
			}
			else if (right.getCost() < left.getCost()) {
				if (right.getCost() < curr.getCost()) {
					swap(index, rightIndex, data);
					heapify(rightIndex, data);
				}
			}
		}
		else { // right child does not exist
			if (left.getCost() < curr.getCost()) {
				swap(index, leftIndex, data);
				heapify(leftIndex, data);
			}
		}
	}
}

void MatchComputer::swap(int a, int b, vector<ComponentMatch> &data) {
	ComponentMatch temp = data[a];
	data[a] = data[b];
	data[b] = temp;
}

void MatchComputer::matchGreedy() {
	// make min heap but associate it to cost	
}