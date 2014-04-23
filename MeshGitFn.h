#pragma once
#include "MeshGitNode.h"
#include <maya/MFnBase.h>
#include <maya/MString.h>
#include <maya/MFnDependencyNode.h>
//#include "MeshGitNode.h"
#include <maya/MGlobal.h>
#include <vector>
#include "EditOperation.h"

using namespace std;

class MeshGitNode;

struct Matched {
	vector<ComponentMatch*> dA_bestMatches;
	vector<ComponentMatch*> dB_bestMatches;
};

struct Unmatched {
	MPointArray* dA_unmatchedPointsOrig;
	MPointArray* dA_unmatchedPointsA; 
	MPointArray* dB_unmatchedPointsOrig;
	MPointArray* dB_unmatchedPointsB;
};

class MeshGitFn: public MFnDependencyNode
{
public:
	MeshGitFn(void);
	~MeshGitFn(void);

	void testMethod(MString test);
	
	//over riding this method so that we can have direct access to the MeshGitFilterNode below that it is attached to!
	virtual MStatus setObject(MObject & object);
	MeshGitNode *meshGitNode;
	void reportError(MStatus status );

	vector<MPointArray*> getAllVerts();
	Matched getMatched();
	Unmatched getUnmatched();
	vector<MFnMesh*> getMFnMeshObjects();

	vector<EditOperation*> getNonconflictingEdits();

	/*void getBestComponentMatches(vector<ComponentMatch*> &dA_bestMatches, vector<ComponentMatch*> &dB_bestMatches);
	void getUnmatchedOriginalMeshPoints(MPointArray* &dA_unmatchedPointsOrig, MPointArray* &dB_unmatchedPointsOrig);
	void getUnmatchedDerivativeMeshPoints(MPointArray* &dA_unmatchedPointsA, MPointArray* &dB_unmatchedPointsB);*/
	void startDiff();
};

