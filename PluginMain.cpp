#include <maya/MPxCommand.h>
#include <maya/MFnPlugin.h>
#include <maya/MIOStream.h>
#include <maya/MString.h>
#include <maya/MArgList.h>
#include <maya/MGlobal.h>
#include <maya/MSimple.h>
#include <maya/MDoubleArray.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MFnNurbsCurve.h>
#include <maya/MDGModifier.h>
#include <maya/MPlugArray.h>
#include <maya/MVector.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MStringArray.h>
#include <list>

#include "MeshGitCmd.h"
#include "MeshGitNode.h"
#include "MeshGitLocatorNode.h"

MStatus initializePlugin( MObject obj )
{
	std::cout << "Loading " << std::endl;
    MStatus status = MStatus::kSuccess;
    MFnPlugin plugin( obj, "LSystemPlugin", "1.0", "Any");
	
	MString pluginPath = plugin.loadPath();

	//Register command
	status = plugin.registerCommand("MeshGitCmd",
    MeshGitCmd::creator, MeshGitCmd::newSyntax);

	//Register Node
	status = plugin.registerNode("MeshGitNode", MeshGitNode::id,
						 MeshGitNode::creator, MeshGitNode::initialize, MPxNode::kDeformerNode);
	status = plugin.registerNode("MeshGitLocatorNode", MeshGitLocatorNode::id,
						 MeshGitLocatorNode::creator, MeshGitLocatorNode::initialize, MPxNode::kLocatorNode);
	if (!status) {
		status.perror("registerNode");
	}

	//Call MELScript file
	MString path = "source \"" + plugin.loadPath() + "/MeshGit.mel\";";
	MGlobal::executeCommand(path);

	//std::cout << "SUCCESSFULLY LOADED MESGIT " << std::endl;
	//MGlobal::displayInfo("SUCCESSFULLY LOADED MESGIT");

    return status;
}

MStatus uninitializePlugin( MObject obj)
{
    MStatus   status = MStatus::kSuccess;
    MFnPlugin plugin( obj );

	status = plugin.deregisterCommand("MeshGitCmd");
	status = plugin.deregisterNode(MeshGitNode::id);
	status = plugin.deregisterNode(MeshGitLocatorNode::id);
	if (!status) {
		status.perror("deregisterNode");
	}

	MString unloadCmd = "unloadMenu;";
	MGlobal::executeCommand(unloadCmd);

	//std::cout << "SUCCESSFULLY DEREGISTERED MESGIT " << std::endl;
	//MGlobal::displayInfo("SUCCESSFULLY DEREGISTERED MESGIT");
    return status;
}


