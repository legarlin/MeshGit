#include "MeshGitFn.h"
#include <maya/MGlobal.h>
#include <maya/MString.h>

using namespace std;

MeshGitFn::MeshGitFn(void)
{
}

MeshGitFn::~MeshGitFn(void) 
{ 
}

void MeshGitFn::testMethod(MString test) 
{
	MGlobal::displayInfo(test);
}

MStatus MeshGitFn::setObject(MObject & object) 
{
	MStatus status;
	status = MFnDependencyNode::setObject(object); // call the super's implementation
	
	MPxNode* genericMeshGitNode = userNode(&status); // set the pointer to our internal object
	reportError(status);

	MGlobal::displayInfo("Node pointer name: "  + genericMeshGitNode->name());

	MeshGitNode* specificMeshGitNode = dynamic_cast<MeshGitNode*> (genericMeshGitNode);

	if (!specificMeshGitNode) {
		MGlobal::displayInfo("Failed to set MeshGitNode pointer in MeshGitFn");
	}
	else {
		MGlobal::displayInfo("Set MeshGitNode pointer in MeshGitFn");
		meshGitNode = specificMeshGitNode;
	}

	return status;
}


void MeshGitFn::reportError(MStatus status ) 
{
	if(status != MStatus::kSuccess) {
		MGlobal::displayInfo("ERROR " + status.errorString());
	}
}

vector<MPointArray*> MeshGitFn::getAllVerts()
{
	//meshGitNode->printTEST();
	return meshGitNode->getAllVerts();
}

vector<MFnMesh*> MeshGitFn::getMFnMeshObjects()
{
	//meshGitNode->printTEST();
	return meshGitNode->getMFnMeshObjects();
}

Matched MeshGitFn::getMatched() 
{
	Matched m;
	m.dA_bestMatches = meshGitNode->dA_bestMatches;
	m.dB_bestMatches = meshGitNode->dB_bestMatches;

	return m;
}

Unmatched MeshGitFn::getUnmatched() 
{
	Unmatched m;
	m.dA_unmatchedPointsOrig = meshGitNode->dA_unmatchedPointsOrig;
	m.dB_unmatchedPointsOrig = meshGitNode->dB_unmatchedPointsOrig;
	m.dA_unmatchedPointsA = meshGitNode->dA_unmatchedPointsA;
	m.dB_unmatchedPointsB = meshGitNode->dB_unmatchedPointsB;

	return m;
}

vector<EditOperation*> MeshGitFn::getNonconflictingEdits()
{
	return meshGitNode->nonconflictingOriginalVerts;
}

vector<EditOperation*> MeshGitFn::getConflictingEdits()
{
	return meshGitNode->conflictingOriginalVerts;
}


//void MeshGitFn::getBestComponentMatches(vector<ComponentMatch*> &dA_bestMatches, vector<ComponentMatch*> &dB_bestMatches){
//	dA_bestMatches = meshGitNode->dA_bestMatches;
//	dB_bestMatches = meshGitNode->dB_bestMatches;
//}
//void MeshGitFn::getUnmatchedOriginalMeshPoints(MPointArray* &dA_unmatchedPointsOrig, MPointArray* &dB_unmatchedPointsOrig){
//	dA_unmatchedPointsOrig = &(meshGitNode->dA_unmatchedPointsOrig);
//	dB_unmatchedPointsOrig = meshGitNode->dB_unmatchedPointsOrig;
//}
//void MeshGitFn::getUnmatchedDerivativeMeshPoints(MPointArray* &dA_unmatchedPointsA, MPointArray* &dB_unmatchedPointsB){
//	dA_unmatchedPointsA = meshGitNode->dA_unmatchedPointsA;
//	dB_unmatchedPointsB = meshGitNode->dB_unmatchedPointsB;
//}

void MeshGitFn::startDiff()
{
	meshGitNode->startDiff();
}

void MeshGitFn::startMergeUnconflicting()
{
	meshGitNode->mergeUnconflicting();
}

int MeshGitFn::findSelectedEditIndex()
{
	return meshGitNode->findSelectedEditIndex();
}

bool MeshGitFn::getCurrentlySelectedEditPositions(MPoint& orginal, MPoint & derivA, MPoint &derivB){

	return meshGitNode->getCurrentlySelectedEditPositions(orginal,  derivA, derivB);

}