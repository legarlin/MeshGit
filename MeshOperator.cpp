#include "MeshOperator.h"

MeshOperator::MeshOperator() { }

MeshOperator:: MeshOperator(vector<MPointArray*> meshVerts, vector<MFnMesh*> fnMeshObjects)
{
	meshVertsOrig = meshVerts[0];
	meshVertsA = meshVerts[1];
	meshVertsB = meshVerts[2];

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