#include "MeshGitFn.h"
#include "MeshGitLocatorNode.h"
#include "MeshGitCmd.h"
#include <maya/MGlobal.h>
#include <maya/MSyntax.h>
#include <maya/MArgDatabase.h>
#include <maya/MPlugArray.h>
#include <maya/MSelectionList.h>
#include <maya/MDGModifier.h>
#include <maya/MDagModifier.h>
#include <list>
using namespace std;
#include <sstream> 

//Command line flags
const char *originalFlag = "-o", *originalLongFlag = "-original";
const char *aFlag = "-a", *aLongFlag = "-derivativeA";
const char *bFlag = "-b", *bLongFlag = "-derivativeB";
const char *connectFlag = "-c", *connectLongFlag= "-connectNodes";
const char *diffFlag = "-d", *diffLongFlag= "-startDiff";

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
	MString meshGitNodeName;
	MString locatorNodeName;

	//Set flags
	if (argData.isFlagSet(originalFlag)) { argData.getFlagArgument(originalFlag, 0, originalFilepath); }
	if (argData.isFlagSet(aFlag)) { argData.getFlagArgument(aFlag, 0, aFilepath); }
	if (argData.isFlagSet(bFlag)) { argData.getFlagArgument(bFlag, 0, bFilepath); }
	if (argData.isFlagSet(connectFlag)) { 
		argData.getFlagArgument(connectFlag, 0, meshGitNodeName); 
		argData.getFlagArgument(connectFlag, 1, locatorNodeName);
		connectNodes(meshGitNodeName, locatorNodeName);
	}
	if (argData.isFlagSet(diffFlag)) { 
		argData.getFlagArgument(diffFlag, 0, meshGitNodeName); 
		startDiff(meshGitNodeName);
	}


	MGlobal::displayInfo(originalFilepath);

    return MStatus::kSuccess;
}

MSyntax MeshGitCmd::newSyntax()
{
    MSyntax syntax;

	syntax.addFlag(originalFlag, originalLongFlag, MSyntax::kString);
	syntax.addFlag(aFlag, aLongFlag, MSyntax::kString);
	syntax.addFlag(bFlag, bLongFlag, MSyntax::kString);
	syntax.addFlag(connectFlag, connectLongFlag, MSyntax::kString, MSyntax::kString);
	syntax.addFlag(diffFlag, diffLongFlag, MSyntax::kString);
	return syntax;
}

std::string stringify(double x)
 {
	std::stringstream s;
	s << "(" << x << ")";
	return s.str();

 }

void MeshGitCmd::startDiff(MString nodeName){
	MGlobal::displayInfo("Starting diff on Node:  " + nodeName );
	MStatus status;

	//Get Node Object
	MSelectionList nodeList;
	status = nodeList.add(nodeName);
	MObject nodeObject;
	status = nodeList.getDependNode(0, nodeObject);
	reportError(status);

	//Create Node Fn
	MeshGitFn mgFn;
	status = mgFn.setObject(nodeObject);
	reportError(status);

	//Start the diff
	mgFn.startDiff();

	//Get the node plug
	MPlug nodePlug = mgFn.findPlug("message", true, &status);
	reportError(status);


}

void MeshGitCmd::connectNodes(MString nodeName, MString locatorName){
	MGlobal::displayInfo("Connecting " + nodeName + " and " + locatorName);
	MStatus status;

	//Get Node Object
	MSelectionList nodeList;
	status = nodeList.add(nodeName);
	MObject nodeObject;
	status = nodeList.getDependNode(0, nodeObject);
	reportError(status);

	//Get Viz Object
	MSelectionList vizList;
	status = vizList.add(locatorName);
	MObject vizObject;
	status = vizList.getDependNode(0, vizObject);
	reportError(status);

	//Create Node Fn
	MeshGitFn mgFn;
	status = mgFn.setObject(nodeObject);
	reportError(status);
	
	//Create viz fn
	MFnDependencyNode vizFn(vizObject, &status);

	//Get the node plug
	MPlug nodePlug = mgFn.findPlug("message", true, &status);
	reportError(status);

	//Get the viz plug 
	MPlug vizPlug = vizFn.findPlug(MeshGitLocatorNode::meshGitNodeConnection, true,
            &status);

	//Connect the plugs 
	
	MDagModifier modifier;
	status = modifier.connect(nodePlug,vizPlug);
	reportError(status);
	status = modifier.doIt();

}

void MeshGitCmd::reportError(MStatus status ){
	if(status != MStatus::kSuccess){
		MGlobal::displayInfo("ERROR " + status.errorString());
	}
}

