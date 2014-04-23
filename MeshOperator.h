#pragma once

#include <vector>
#include <maya/MPointArray.h>
#include <maya/MFnMeshData.h>
#include <maya/MFnMesh.h>
#include <maya/MVector.h>

#include "MatchComputer.h"

using namespace std;

class MeshOperator {
	MPointArray* meshVertsOrig;
	MPointArray* meshVertsA;
	MPointArray* meshVertsB;

	MFnMesh* fnMeshOrig;
	MFnMesh* fnMeshA;
	MFnMesh* fnMeshB;

	MatchComputer* matchComputerA;
	MatchComputer* matchComputerB;

public:
	vector<ComponentMatch*> dA_bestMatches;
	MPointArray* dA_unmatchedPointsOrig;
	MPointArray* dA_unmatchedPointsA;

	vector<ComponentMatch*> dB_bestMatches;
	MPointArray* dB_unmatchedPointsOrig;
	MPointArray* dB_unmatchedPointsB;

	MeshOperator();
	MeshOperator(vector<MPointArray*> meshVerts, vector<MFnMesh*> fnMeshObjects);

	void diff();
};