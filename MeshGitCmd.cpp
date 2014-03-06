#include "MeshGitCmd.h"
#include <maya/MGlobal.h>
#include <maya/MSyntax.h>
#include <maya/MArgDatabase.h>
#include <list>
using namespace std;
#include <sstream> 

//Command line flags
const char *originalFlag = "-o", *originalLongFlag = "-original";
const char *aFlag = "-a", *aLongFlag = "-derivativeA";
const char *bFlag = "-b", *bLongFlag = "-derivativeB";

MeshGitCmd::MeshGitCmd() : MPxCommand()
{

}

MeshGitCmd::~MeshGitCmd() 
{
}

MStatus MeshGitCmd::doIt( const MArgList& args )
{
	MStatus status;

	MArgDatabase argData( syntax(), args, &status );
    if (!status)
        return status;

	MString originalFilepath = "";
	MString aFilepath = "";
	MString bFilepath = "";

	//Set flags
	if (argData.isFlagSet(originalFlag)) { argData.getFlagArgument(originalFlag, 0, originalFilepath); }
	if (argData.isFlagSet(aFlag)) { argData.getFlagArgument(aFlag, 0, aFilepath); }
	if (argData.isFlagSet(bFlag)) { argData.getFlagArgument(bFlag, 0, bFilepath); }

	MGlobal::displayInfo(originalFilepath);

    return MStatus::kSuccess;
}

MSyntax MeshGitCmd::newSyntax()
{
    MSyntax syntax;

	syntax.addFlag(originalFlag, originalLongFlag, MSyntax::kString);
	syntax.addFlag(aFlag, aLongFlag, MSyntax::kString);
	syntax.addFlag(bFlag, bLongFlag, MSyntax::kString);

	return syntax;
}

std::string stringify(double x)
 {
	std::stringstream s;
	s << "(" << x << ")";
	return s.str();

 }


