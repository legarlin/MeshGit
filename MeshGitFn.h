#pragma once
#include "MeshGitNode.h"
#include <maya/MFnBase.h>
#include <maya/MString.h>
#include <maya/MFnDependencyNode.h>
//#include "MeshGitNode.h"
#include <maya/MGlobal.h>
#include <vector>

using namespace std;

class MeshGitNode;
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
	void getAllVerts(std::vector<MPointArray> &points);
};

