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

void MeshGitFn::testMethod(MString test){
	MGlobal::displayInfo(test);
}

MStatus MeshGitFn::setObject(MObject & object){
	MStatus status;

	//call the super's implementation
	status = MFnDependencyNode::setObject(object);

	//Set the pointer to our internal object
	MPxNode *genericMeshGitNode = userNode(&status);
	reportError(status);
	MGlobal::displayInfo("Node pointer name: "  + genericMeshGitNode->name());
	MeshGitNode *specificMeshGitNode = dynamic_cast<MeshGitNode *> (genericMeshGitNode);
	if (!specificMeshGitNode) 
		MGlobal::displayInfo("Failed to set MeshGitNode pointer in MeshGitFn");
	else{
		MGlobal::displayInfo("Set MeshGitNode pointer in MeshGitFn");
		meshGitNode = specificMeshGitNode;
	}

	return status;
}


void MeshGitFn::reportError(MStatus status ){
	if(status != MStatus::kSuccess){
		MGlobal::displayInfo("ERROR " + status.errorString());
	}
}

void MeshGitFn::getAllVerts(std::vector<MPointArray> &points){
	meshGitNode->printTEST();
	meshGitNode->getAllVerts(points);
}