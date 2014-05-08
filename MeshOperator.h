#pragma once

#include <vector>
#include <maya/MPointArray.h>
#include <maya/MFnMeshData.h>
#include <maya/MFnMesh.h>
#include <maya/MVector.h>

#include "MatchComputer.h"
#include "EditOperation.h"
#include "MeshComponent.h"
#include "ComponentMatch.h"
#include <maya/MStringArray.h>

using namespace std;

class MeshOperator {
	MPointArray* meshVertsOrig;
	MPointArray* meshVertsA;
	MPointArray* meshVertsB;
	MPointArray* meshVertsOutput;

	MFnMesh* fnMeshOrig;
	MFnMesh* fnMeshA;
	MFnMesh* fnMeshB;
	MFnMesh* fnMeshOutput;


	MatchComputer* matchComputerA;
	MatchComputer* matchComputerB;

	vector<MeshComponent*> origMeshComponents;

public:
	vector<ComponentMatch*> dA_bestMatches;
	MPointArray* dA_unmatchedPointsOrig;
	MPointArray* dA_unmatchedPointsA;

	vector<ComponentMatch*> dB_bestMatches;
	MPointArray* dB_unmatchedPointsOrig;
	MPointArray* dB_unmatchedPointsB;

	vector<EditOperation*> allEdits;
	vector<EditOperation*> nonconflictingEdits;
	vector<EditOperation*> conflictingEdits;
	vector<MString> editInfo;
	void updateEditStrings();


	MeshOperator();
	MeshOperator(vector<MPointArray*> meshVerts, vector<MFnMesh*> fnMeshObjects);

	void makeComponents();
	void diff();
	void checkConflicts();
	MPointArray* mergeUnconflictingEdits();
};