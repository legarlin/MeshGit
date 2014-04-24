#include "MeshOperator.h"

MeshOperator::MeshOperator() { }

MeshOperator:: MeshOperator(vector<MPointArray*> meshVerts, vector<MFnMesh*> fnMeshObjects)
{
	meshVertsOrig = meshVerts[0];
	meshVertsA = meshVerts[1];
	meshVertsB = meshVerts[2];

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

		bool conflicting = edit->conflict;
		if (!conflicting) {
			nonconflictingEdits.push_back(edit);
		}
		else 
			conflictingEdits.push_back(edit);

		MString editString = "Component Number: ";
		editString += i;
		editString+=  " | Status: ";
		if(conflicting)
			editString+= " CONFLICT!";
		else
			editString+= " no conflict";
		cout<<editString << endl;
		editInfo.push_back(editString);


	}
}