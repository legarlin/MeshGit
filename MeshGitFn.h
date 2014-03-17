#pragma once
#include "MeshGitNode.h"
#include <maya/MFnBase.h>
#include <maya/MString.h>
#include <maya/MFnDependencyNode.h>

class MeshGitFn: public MFnDependencyNode
{
public:
	MeshGitFn(void);
	~MeshGitFn(void);


	void testMethod(MString test);
	//virtual MStatus setObject(MObject & object);
    /*BEGIN_NO_SCRIPT_SUPPORT:
    virtual MStatus setObject( const MObject & object );
    END_NO_SCRIPT_SUPPORT:*/
};

