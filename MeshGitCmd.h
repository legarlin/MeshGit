#ifndef CreateMeshGitCmd_H_
#define CreateMeshGitCmd_H_

#include <maya/MPxCommand.h>
#include <string>

class MeshGitCmd : public MPxCommand
{
public:
    MeshGitCmd();
    virtual ~MeshGitCmd();
    static void* creator() { return new MeshGitCmd(); }
    MStatus doIt( const MArgList& args );
    static MSyntax newSyntax();
	void connectNodes(MString nodeName, MString locatorName);
	void reportError(MStatus status);
	void startDiff(MString nodeName);
};

#endif