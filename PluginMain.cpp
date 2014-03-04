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

MStatus initializePlugin( MObject obj )
{
	std::cout << "Loading " << std::endl;
    MStatus status = MStatus::kSuccess;
    MFnPlugin plugin( obj, "LSystemPlugin", "1.0", "Any");
	
	MString pluginPath = plugin.loadPath();

	//Loading the mel file script
	//MString cmd = "source \""+ pluginPath + "/" + "MeshGit.mel\"";
	//MGlobal::executeCommand(cmd);
	//	char buffer[2048];
	//sprintf_s(buffer, 2048, "source \"%s/LSystem.mel\";", pluginPath);
	//MGlobal::executeCommand(buffer, true);

	//Register Node
	status = plugin.registerNode("MeshGitNode", MeshGitNode::id,
						 MeshGitNode::creator, MeshGitNode::initialize);
	if (!status) {
		status.perror("registerNode");
	}

	std::cout << "SUCCESSFULLY LOADED MESGIT " << std::endl;
	MGlobal::displayInfo("SUCCESSFULLY LOADED MESGIT");

    return status;
}

MStatus uninitializePlugin( MObject obj)
{
    MStatus   status = MStatus::kSuccess;
    MFnPlugin plugin( obj );

	status = plugin.deregisterNode(MeshGitNode::id);
	if (!status) {
		status.perror("deregisterNode");
	}

	std::cout << "SUCCESSFULLY DEREGISTERED MESGIT " << std::endl;
	MGlobal::displayInfo("SUCCESSFULLY DEREGISTERED MESGIT");
    return status;
}


