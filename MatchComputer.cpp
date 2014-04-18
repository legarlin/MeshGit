#include "MatchComputer.h"
#include <maya/MIntArray.h>

#include <queue>

MatchComputer::MatchComputer() { }

MatchComputer::MatchComputer(MPointArray* originalVerts, MPointArray* derivativeVerts, MFnMesh* oMeshFn , MFnMesh* dMeshFn) {
	iterationCount = 0;
	originalMeshFn=oMeshFn;
	derivativeMeshFn=dMeshFn;
	makeComponents(originalVerts, derivativeVerts);

	//initially has everything, but then later we remove as the algo goes on. 
	unmatchedOriginalMeshPoints = new MPointArray(*originalVerts);
	unmatchedDerivativeMeshPoints = new MPointArray(*derivativeVerts);

	int numPolygons = originalMeshFn->numPolygons();
	cout<<"Num Polygons " << numPolygons <<endl;

	makeComponentMatches();
	matchGreedy();
}


void MatchComputer::makeComponents(MPointArray* originalVerts, MPointArray* derivativeVerts){
	// Convert verts into components and store them, not doing faces yet

	//Make Vertex Components for Original Mesh
	for (unsigned int i = 0; i < originalVerts->length(); i++) {
		//cout<<originalVerts[i]<< endl;
		MeshComponent* component = new MeshComponent(MeshComponent::VERTEX,(*originalVerts)[i]);
		originalMeshComponents.push_back(component);
	}

	cout<<"Original Verts " << *originalVerts <<endl;
	//Make Face Components for Original Mesh
	int numPolygons = originalMeshFn->numPolygons();
	cout<<"Num Polygons " << numPolygons <<endl;
	for(int i = 0 ; i <numPolygons; i++){
		MIntArray points;
		originalMeshFn->getPolygonVertices(i,points);
		cout<<"POLYGON " << i<< "    " << points<<endl;


		MeshComponent* component = new MeshComponent(MeshComponent::FACE,(*originalVerts)[i]);
		

		originalMeshComponents.push_back(component);
	}

	//Make Vertex Components for Derivative Mesh
	for (unsigned int i = 0; i < derivativeVerts->length(); i++) {
		//cout<<derivativeVerts[i]<< endl;
		MeshComponent* component = new MeshComponent(MeshComponent::VERTEX,(*derivativeVerts)[i]);
		derivativeMeshComponents.push_back(component);	
	}

	numUnmatched = originalMeshComponents.size() + derivativeMeshComponents.size();
}


void MatchComputer::makeComponentMatches(){

	//Create all pairings betewen original components and deriv components, place them into allComponentMatches
	//these will replace some of the unmatched ones from the next step
	for (int i = 0; i < originalMeshComponents.size() ; i++){
		for (int j = 0; j < derivativeMeshComponents.size() ; j++){
			MeshComponent* currentCompA = originalMeshComponents[i];
			MeshComponent* currentCompB = derivativeMeshComponents[j];

			ComponentMatch* newCompMatch = new ComponentMatch(currentCompA,currentCompB);
			allComponentMatches.push_back(newCompMatch);
		}
	}

	//COMMENTED OUT, CHANGED IT SO THAT UNMATCHED COSTS ARE TAKEN ACCOUNT OF IN THE COMPUTE COST METHOD
	//Create all unmatched componentMatches and add them into allComponentMatches
	//these matches are still valid, and might be better initially
	//for (int i = 0; i < originalMeshComponents.size(); i++){
	//	MeshComponent currentCompA = originalMeshComponents[i];
	//	MeshComponent emptyComp;
	//	emptyComp.type = MeshComponent::EMPTY;
	//	ComponentMatch newCompMatch(currentCompA,emptyComp);
	//	allComponentMatches.push_back(newCompMatch);
	//}
	//for (int j = 0; j < derivativeMeshComponents.size(); j++){
	//	MeshComponent currentCompB = derivativeMeshComponents[j];
	//	MeshComponent emptyComp;
	//	emptyComp.type = MeshComponent::EMPTY;
	//	ComponentMatch newCompMatch(emptyComp,currentCompB);
	//	allComponentMatches.push_back(newCompMatch);
	//}
}





void MatchComputer::makeHeap(vector<ComponentMatch*> &data) {
	int index = (int) data.size() / 2 - 1;
	for (int i = index; i >= 0; i--) {
		heapify(i, data);
	}
}

void MatchComputer::heapify(int index, vector<ComponentMatch*> &data) {
	int leftIndex = index*2 + 1; // left
	int rightIndex = index*2 + 2; // right

	if (leftIndex < data.size()) {
		ComponentMatch* curr = data[index];
		ComponentMatch* left = data[leftIndex];

		if (rightIndex < data.size()) { // check if right child node exists
			ComponentMatch* right = data[rightIndex];

			if (left->getCost() < right->getCost()) {
				if (left->getCost() < curr->getCost()) {
					swap(index, leftIndex, data);
					heapify(leftIndex, data);
				}
			}
			else if (right->getCost() < left->getCost()) {
				if (right->getCost() < curr->getCost()) {
					swap(index, rightIndex, data);
					heapify(rightIndex, data);
				}
			}
		}
		else { // right child does not exist
			if (left->getCost() < curr->getCost()) {
				swap(index, leftIndex, data);
				heapify(leftIndex, data);
			}
		}
	}
}

void MatchComputer::swap(int a, int b, vector<ComponentMatch*> &data) {
	ComponentMatch* temp = data[a];
	data[a] = data[b];
	data[b] = temp;
}

void MatchComputer::matchGreedy() {
	iterationCount++;
	//cout<<"Starting Iteration " << iterationCount << endl;
	if(allComponentMatches.size() == 0)
		return;

	//Calculate old cost 
	double oldCost = computeCost(bestComponentMatches, numUnmatched);

	//Pick lowest cost match from all costs
	ComponentMatch* lowestMatch = getAndRemoveLowestComponentMatch();
	//Check if lowestMatch components are already in lowestComponents - do we need to do this? 
	//if so go to next iteration since they were already assigned
	if(areMatchComponentsInVector(lowestMatch, bestComponentMatches)){
		return matchGreedy();
	}
	
	//Find out total cost of using this new match
	vector<ComponentMatch*> newBestComponentMatches(bestComponentMatches);	//make copy of lowestComponentMatches
	newBestComponentMatches.push_back(lowestMatch);	//insert new lowestMatch
	//compute possible new cost
	double newCost = computeCost(newBestComponentMatches, numUnmatched-2);

	//COMPARE THE OLD AND NEW COSTS
	if(newCost < oldCost){
		//copy over the new lowest matches etc . 
		bestComponentMatches = newBestComponentMatches;
		numUnmatched= numUnmatched-2;

		//remove from unmatched list since these elements are now matched!
		removeFromUnmatched(lowestMatch);

		//go to next iteration		
		return matchGreedy();
	}

	//if not them end the greedy algorithm	
	return;
}


bool MatchComputer::areMatchComponentsInVector(ComponentMatch* match, vector<ComponentMatch*> matches){
	MeshComponent* compA = match->originalComp;
	MeshComponent* compB = match->derivativeComp;
	
	for(int i= 0 ; i<matches.size(); i ++){
		ComponentMatch* current = matches[i];
		if(compA->isEqualTo(current->originalComp) || compB->isEqualTo(current->derivativeComp))
			return true;
	}

	return false; 
}

void MatchComputer::removeFromUnmatched(ComponentMatch* match){
	MeshComponent* compA = match->originalComp;
	MeshComponent* compB = match->derivativeComp;
	//now all verts so just get those
	MPoint vA = compA->pos;
	MPoint vB = compB->pos;

	int Olength = unmatchedOriginalMeshPoints->length();

	for(int i = 0; i < Olength; i ++) {
		MPoint test = (*unmatchedOriginalMeshPoints)[i];

		if((*unmatchedOriginalMeshPoints)[i] == vA) {
			unmatchedOriginalMeshPoints->remove(i);
			break;
		}
	}

	int Dlength = unmatchedDerivativeMeshPoints->length();

	for(int i = 0; i < Dlength; i ++) {
		if((*unmatchedDerivativeMeshPoints)[i] == vB) {
			unmatchedDerivativeMeshPoints->remove(i);
			break;
		}
	}
}


double MatchComputer::computeCost(vector<ComponentMatch*> matches, int numUnmatchedArg){
	double cost = 0.0;

	//All the matched costs
	for(int i = 0; i < matches.size(); i++){
		cost += matches[i]->getCost();
	}

	//All the unmatched costs
	cost += numUnmatchedArg*1.0;

	return cost; 
}

//Will replace this with the minheap above later
ComponentMatch* MatchComputer::getAndRemoveLowestComponentMatch(){

	double lowestCost=9999;
	ComponentMatch* lowestMatch;
	int lowestIndex=-1;
	for(int i = 0; i <allComponentMatches.size(); i++){
		double currentCost = allComponentMatches[i]->getCost();
		if(currentCost<lowestCost){
			lowestMatch = allComponentMatches[i];
			lowestIndex = i;
			lowestCost = currentCost;
		}
	}

	//this only happens allComponentMatches was empty, it shouldnt be
	if(lowestIndex == -1)
		return lowestMatch;

	//Remove the lowest match from the vector
	allComponentMatches.erase(allComponentMatches.begin() + lowestIndex);
	return lowestMatch; //does deleting it mean this still exists?

}