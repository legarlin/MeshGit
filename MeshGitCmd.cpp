#include "MeshGitCmd.h"
#include <maya/MGlobal.h>
#include <maya/MSyntax.h>
#include <maya/MArgDatabase.h>
#include <list>
using namespace std;
#include <sstream> 

//Comand line flags
const char *defaultStepSizeFlag = "-s", *defaultStepSizeLongFlag = "-stepSize";
const char *defaultAngleFlag = "-a", *defaultAngleLongFlag = "-angleSize";
const char *defaultGrammarFlag = "-g", *defaultGrammarLongFlag = "-grammar";
const char *defaultNumberOfIterationsFlag = "-i", *defaultNumberOfIterationsLongFlag = "-numberOfIterations";

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

	//Checking if flags are set
	//if( argData.isFlagSet( defaultStepSizeFlag ) )
	//	argData.getFlagArgument( defaultStepSizeFlag, 0, defaultStep );
	//if( argData.isFlagSet( defaultAngleFlag ) )
	//	argData.getFlagArgument( defaultAngleFlag, 0, angle );
	//if( argData.isFlagSet( defaultGrammarFlag ) )
	//	argData.getFlagArgument( defaultGrammarFlag, 0, grammar );
	//if( argData.isFlagSet( defaultNumberOfIterationsFlag ) )
	//	argData.getFlagArgument( defaultNumberOfIterationsFlag, 0, angle );

    return MStatus::kSuccess;
}

MSyntax MeshGitCmd::newSyntax()
{
    MSyntax syntax;

    syntax.addFlag( defaultStepSizeFlag, defaultStepSizeLongFlag, MSyntax:: kDouble );
    syntax.addFlag( defaultAngleFlag, defaultAngleLongFlag, MSyntax::kDouble );
	syntax.addFlag( defaultGrammarFlag, defaultGrammarLongFlag, MSyntax::kString );
	syntax.addFlag( defaultNumberOfIterationsFlag, defaultNumberOfIterationsLongFlag, MSyntax::kDouble);

	return syntax;
}

std::string stringify(double x)
 {
	std::stringstream s;
	s << "(" << x << ")";
	return s.str();

 }


