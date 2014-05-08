#include "MeshOperator.h"

MeshOperator::MeshOperator() { }

MeshOperator:: MeshOperator(vector<MPointArray*> meshVerts, vector<MFnMesh*> fnMeshObjects)
{
	meshVertsOrig = meshVerts[0];
	meshVertsA = meshVerts[1];
	meshVertsB = meshVerts[2];
	meshVertsOutput = meshVerts[3];
	//cout << "meshVertsBXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" << endl;
	//cout<< *meshVertsB << endl;
	

	fnMeshOrig = fnMeshObjects[0];
	fnMeshA = fnMeshObjects[1];
	fnMeshB = fnMeshObjects[2];
}

void MeshOperator::diff()
{
	matchComputerA = new MatchComputer(meshVertsOrig, meshVertsA, fnMeshOrig, fnMeshA);
	matchComputerB = new MatchComputer(meshVertsOrig, meshVertsB, fnMeshOrig, fnMeshB);

	dA_bestMatches = matchComputerA->bestComponentMatches;
	dA_unmatchedPointsOrig = matchComputerA->unmatchedOriginalMeshPoints;
	dA_unmatchedPointsA = matchComputerA->unmatchedDerivativeMeshPoints;

	dB_bestMatches = matchComputerB->bestComponentMatches;
	dB_unmatchedPointsOrig = matchComputerB->unmatchedOriginalMeshPoints;
	dB_unmatchedPointsB = matchComputerB->unmatchedDerivativeMeshPoints;
}

void MeshOperator::checkConflicts()
{
	vector<MeshComponent*> origMeshComponents_A = matchComputerA->originalMeshComponents;
	vector<MeshComponent*> origMeshComponents_B = matchComputerB->originalMeshComponents;

	if (origMeshComponents_A.size() != origMeshComponents_B.size()) {
		cout << "uh oh." << endl;
		return;
	}

	unsigned int i, size_orig;
	size_orig = origMeshComponents_A.size();
	for (i = 0; i < size_orig; ++i) {
		MeshComponent* mc_A = origMeshComponents_A[i];
		MeshComponent* mc_B = origMeshComponents_B[i];

		if (!mc_A->matched) break;
		if (!mc_B->matched) break;

		ComponentMatch* cm_A = mc_A->matched;
		ComponentMatch* cm_B = mc_B->matched;
				
		EditOperation* edit = new EditOperation(cm_A, cm_B);
		allEdits.push_back(edit); 

		bool conflicting = edit->conflict;
		if (!conflicting) {
			nonconflictingEdits.push_back(edit);
		}
		else 
			conflictingEdits.push_back(edit);
	}

	updateEditStrings(); 
}

MPointArray* MeshOperator::mergeUnconflictingEdits(){
	cout<<"Starting Merge unconflicting Edits"<<endl;
	for(int v = 0; v<nonconflictingEdits.size(); v++){
		EditOperation* eo = nonconflictingEdits.at(v); 
		ComponentMatch * editedMatch;
		if(eo->aChanged==true){
			editedMatch=eo->matchA;
		}
		else
			editedMatch= eo->matchB;


		Match m = editedMatch->getMatches();
		MeshComponent * derivComp = m.derivativeComp; 
		MPoint pos = derivComp->pos;
		int index = derivComp->index;
		meshVertsOutput->set(pos,index); 
		eo->resolved=true; 
	}

	//fnMeshOutput->setPoints(*meshVertsOutput, MSpace::kObject); 
	return meshVertsOutput;

	cout<<"Ended Merge unconflicting Edits = count: " << nonconflictingEdits.size()<<endl;
}

void MeshOperator::updateEditStrings(){

	editInfo.clear(); 
	for(int  i= 0; i < allEdits.size(); i ++ ){ 
		
		EditOperation* edit = allEdits[i];

		bool conflicting = edit->conflict;
		bool resolved = edit->resolved;
		MString editString = "Component Number: ";
		editString += i;
		editString+=  " | Status: ";
		if(conflicting)
			editString+= " CONFLICT!|  ";
		else
			editString+= " no conflict |  ";
		if(resolved)
			editString+= " resolved";
		else 
			editString+= " NOT RESOLVED!";

		cout<< editString << endl;
		editInfo.push_back(editString);

	}

}

