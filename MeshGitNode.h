#include <maya/MTime.h>
#include <maya/MFnMesh.h>
#include <maya/MPoint.h>
#include <maya/MFloatPoint.h>
#include <maya/MFloatPointArray.h>
#include <maya/MIntArray.h>
#include <maya/MDoubleArray.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnNumericAttribute.h>
//#include <maya/MFnPlugin.h>

#include <maya/MPxNode.h>
#include <maya/MPxDeformerNode.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MFnMeshData.h>
#include <maya/MIOStream.h>
#include <maya/MItGeometry.h>
#include <maya/MPointArray.h>
#include <vector>
#include <maya/MMatrix.h>
#include "ComponentMatch.h"
#include "MeshOperator.h"
#include "EditOperation.h"

using namespace std;

//HEADER
class MeshGitNode : public MPxDeformerNode
{
public:
					MeshGitNode();
	virtual 		~MeshGitNode();
	//virtual MStatus compute(const MPlug& plug, MDataBlock& data);
    virtual MStatus      		deform(MDataBlock& 		block,
									   MItGeometry& 	iter,									   
									   unsigned int		multiIndex);
	static  void*	creator();
	static  MStatus initialize();
	static MObject	originalMesh;
	static MObject	derivativeAMesh;
	static MObject	derivativeBMesh;
	static MObject	mergedMesh;
	static MObject currentFrame;
	static MObject editStringArray;
	static MTypeId	id;

	//All Data holders
	std::vector<MPointArray*> allVerts;
	MPointArray* mergedVerts;


	std::vector<MMatrix> allTransforms;
	std::vector<ComponentMatch*> dA_bestMatches;
	std::vector<MFnMesh*> allMFnMeshObjects;
	MPointArray* dA_unmatchedPointsOrig;
	MPointArray* dA_unmatchedPointsA;

	std::vector<ComponentMatch*> dB_bestMatches;
	MPointArray* dB_unmatchedPointsOrig;
	MPointArray* dB_unmatchedPointsB;

	vector<MPointArray*> getAllVerts();
	vector<MString> getEditStrings();
	vector<MFnMesh*> getMFnMeshObjects();
	MeshOperator* meshOperator;
	vector<EditOperation*> nonconflictingOriginalVerts;
	vector<EditOperation*> conflictingOriginalVerts;

	MStatus compute(const MPlug& plug, MDataBlock& dataBlock);
	MStatus storeAllVerts(MDataBlock& dataBlock);
	MStatus deformOutputMesh(MDataBlock &dataBlock);
	void startDiff();
	void mergeUnconflicting();
	unsigned int mergedMeshGroupID;
	MString diffResultsWindowName;
	MString diffResultsScrollListName;
	int selectedEditIndex;
	
	void  reloadDiffResultsWindow();
	int  findSelectedEditIndex();

	//PRINTING AND DEBUGGING FUNCTIONS
	void printVectorOfPoints(MString name, std::vector<MPointArray> &points);
	void printTEST();
	void reportError(MStatus status);
	virtual void postConstructor();

	void manualResolveConflict(int index);

protected:
	
};
//END HEADER