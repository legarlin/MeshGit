#include "MatchComputer.h"

#include <queue>

MatchComputer::MatchComputer() { }

MatchComputer::MatchComputer(MPointArray &meshVerts1, MPointArray &meshVerts2) {

	makeComponents(meshVerts1,meshVerts2);
	makeComponentMatches();

}


void MatchComputer::makeComponents(MPointArray &meshVerts1, MPointArray &meshVerts2){
	//Convert verts into components and store them , not doing faces yet
	for (unsigned int i = 0; i < meshVerts1.length(); i++) {
		MeshComponent component(meshVerts1[i]);//is it okay that these aren't pointers?
		originalMeshComponents.push_back(component);
	}

	for (unsigned int i = 0; i < meshVerts2.length(); i++) {
		MeshComponent component(meshVerts2[i]);
		derivativeMeshComponents.push_back(component);
	}


}


void MatchComputer::makeComponentMatches(){

	//Create all pairings betewen original components and deriv components, place them into allComponentMatches
	//these will replace some of the unmatched ones from the next step
	for (int i = 0; i < originalMeshComponents.size() ; i++){
		for (int j = 0; j < derivativeMeshComponents.size() ; j++){
			MeshComponent currentCompA = originalMeshComponents[i];
			MeshComponent currentCompB = derivativeMeshComponents[j];

			ComponentMatch newCompMatch(currentCompA,currentCompB);
			allComponentMatches.push_back(newCompMatch);
		}
	}

	//Create all unmatched componentMatches and add them into lowestComponentMatches
	//initially everything is unmatched!
	for (int i = 0; i < originalMeshComponents.size(); i++){
		MeshComponent currentCompA = originalMeshComponents[i];
		MeshComponent emptyComp;
		emptyComp.type = MeshComponent::EMPTY;
		ComponentMatch newCompMatch(currentCompA,emptyComp);
		lowestComponentMatches.push_back(newCompMatch);
	}
	for (int j = 0; j < derivativeMeshComponents.size(); j++){
		MeshComponent currentCompB = derivativeMeshComponents[j];
		MeshComponent emptyComp;
		emptyComp.type = MeshComponent::EMPTY;
		ComponentMatch newCompMatch(emptyComp,currentCompB);
		lowestComponentMatches.push_back(newCompMatch);
	}

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
	
	//pick lowest cost match from all costs
	



}