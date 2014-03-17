#include "MeshGitFn.h"
#include <maya/MGlobal.h>

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