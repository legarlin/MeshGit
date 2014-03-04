#ifndef CreateLSystemCmd_H_
#define CreateLSystemCmd_H_

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

};

#endif