#include "MatchComputer.h"
#include <maya/MIntArray.h>
#include <map>
#include <queue>

MatchComputer::MatchComputer() { }

MatchComputer::MatchComputer(MPointArray* originalVerts, MPointArray* derivativeVerts, MFnMesh* oMeshFn , MFnMesh* dMeshFn) {
	iterationCount = 0;
	originalMeshFn = oMeshFn;
	derivativeMeshFn = dMeshFn;
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
		MeshComponent* component = new MeshComponent(MeshComponent::VERTEX,(*originalVerts)[i], i);
		originalMeshComponents.push_back(component);
	}
	
	//Make Face Components for Original Mesh
	int numPolygons = originalMeshFn->numPolygons();
	cout<<"Num Polygons " << numPolygons <<endl;
	for(int i = 0 ; i <numPolygons; i++){
		MIntArray points;
		originalMeshFn->getPolygonVertices(i,points);
		cout<<"POLYGON " << i<< "    " << points<<endl;
		MPoint averagePoint(0,0,0,0); 
		MeshComponent* component = new MeshComponent(MeshComponent::FACE,averagePoint, i);//averagePoint is overwritten later
		int numPoints= points.length();
		for(int v = 0; v<numPoints; v++){
			MPoint currentPoint = (*originalVerts)[points[v]];
			MVector vRep(currentPoint);
			averagePoint += vRep;
			//Add in ALL Face-Vertex Adjacency Information as well 
			MeshComponent* vertComponent = originalMeshComponents[points[v]];
			component->addAdjacency(vertComponent,false);

			//Add on all Vert-Vert Adjacency information. All those connected to each other around each face will be adjacent!
			int nextVertIndex = (v+1)%numPoints;
			MeshComponent* vertComponent2 = originalMeshComponents[points[nextVertIndex]];
			vertComponent->addAdjacency(vertComponent2,false);
		}
		averagePoint = averagePoint / points.length();

		component->pos = averagePoint;
		cout<<"Face " << i  << " Position " << averagePoint <<endl;
		originalMeshFaceComponents.push_back(component);
	}

	//Face-Face Adjacencies - Basically we want to make faces which share an edge adjacent
	for(int m = 0 ; m <originalMeshComponents.size(); m++){
		cout<<"STARTING Adjacencies " << m << endl;
		 MeshComponent* currentVertComp = originalMeshComponents[m];
		 std::set<MeshComponent*> adjacentFacesFIRST = currentVertComp->adjacentFaces;
		 std::set<MeshComponent*> adjacentVertsFIRST = currentVertComp->adjacentVertices;
		 cout<<"Got Elements " << adjacentFacesFIRST.size() << " | " << adjacentVertsFIRST.size()<<endl;
		 std::set<MeshComponent*>::iterator it0;


		 for (it0 = adjacentVertsFIRST.begin(); it0 != adjacentVertsFIRST.end(); ++it0){
			 cout<<"it0 " << endl;
			  MeshComponent* secondVertComp = *it0;
			  std::set<MeshComponent*> adjacentFacesSECOND = secondVertComp->adjacentFaces;
			  std::vector<MeshComponent*> adjacentFaces; 
			  //Find out which faces share the current edge and add them into the adjacent face holder
			   std::set<MeshComponent*>::iterator it1;
				for (it1 = adjacentFacesFIRST.begin(); it1 != adjacentFacesFIRST.end(); ++it1)
				{   cout<<"it1 " << endl;
					MeshComponent* af1 = *it1;
					if(adjacentFacesSECOND.count(af1)>0){
						adjacentFaces.push_back(af1);
					}
				}
			    cout<<"Before Adding " << endl;
				for(int x = 0; x<adjacentFaces.size(); x++){
					for(int y = 0; y< adjacentFaces.size(); y++){
						cout<<"Going to add adjacent faces" << endl;
						MeshComponent* face1 = adjacentFaces[x];
						MeshComponent* face2 = adjacentFaces[y];
						cout<<"face1" << face1 << endl;
						cout<<"face2" << face2 << endl;
						face1->addAdjacency(face2,false);
						cout<<"Added Adjacency" << endl;
					}
				}
				cout<<"Finished Adding Adjacency" << endl;
		 }

		 cout<<"ENDING Adjacencies " << m << endl;
	}






	//Make Vertex Components for Derivative Mesh
	for (unsigned int i = 0; i < derivativeVerts->length(); i++) {
		//cout<<derivativeVerts[i]<< endl;
		MeshComponent* component = new MeshComponent(MeshComponent::VERTEX,(*derivativeVerts)[i],  i);
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