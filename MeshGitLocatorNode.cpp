#include <maya/MDagPath.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnMessageAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MMatrix.h>
#include <maya/MPlugArray.h>
#include <MeshGitLocatorNode.h>
#include <maya/MGlobal.h>
#include "MeshGitFn.h"

#define scopeName "MeshGitLocatorNode"
MTypeId MeshGitLocatorNode::id(0x03AA4);

using namespace std;
MObject MeshGitLocatorNode::meshGitNodeConnection;


void * MeshGitLocatorNode::creator() {
    return new MeshGitLocatorNode;
}

MStatus MeshGitLocatorNode::initialize() {
    MStatus status;
    MFnNumericAttribute nAttr;
    MFnMessageAttribute msAttr;
    MFnEnumAttribute eAttr;

	//Create the message attribute which connects to the MeshGitNode (connected by the cmd class) 
	meshGitNodeConnection = msAttr.create("meshGitNodeConnection", "mgC", &status);
    reportError(status);
    status = addAttribute(meshGitNodeConnection);
	reportError(status);
  
    return status;
}

void MeshGitLocatorNode::draw(M3dView & view, const MDagPath & path,
                              M3dView::DisplayStyle ,
                              M3dView::DisplayStatus ) {
    MStatus status;
    MObject thisObject = thisMObject();

    
    //Obtain the display matrix
    MMatrix worldToDisplayViewMatrix = path.inclusiveMatrixInverse(&status);
	reportError(status);
	MGlobal::displayInfo("Got path matrix value");

	//Getting the actual MeshGitNode Object from the plug
	MFnDependencyNode nodeFn(thisObject);
	MPlug plug = nodeFn.findPlug(meshGitNodeConnection, &status);
	reportError(status);
	MPlugArray connectedElements;
	plug.connectedTo(connectedElements, true, false, &status);
	reportError(status);
	//might need to check for case where nothing is connected? but we know it will always be connected so don't worry? 
	//Maya crashes here so check below
	if (!connectedElements.length()){
		MGlobal::displayInfo("No connection on viz node");
	}
	else{	
		MObject meshGitNodeObject = connectedElements[0].node();
		MGlobal::displayInfo("Got meshgitnode object :"+ connectedElements[0].name());
		//Attach a fnset to the object so that we can access what is underneath
		MeshGitFn mgFn;
		status = mgFn.setObject(meshGitNodeObject);
		reportError(status);
		MGlobal::displayInfo("Attached fnsetobject to the nodeobject");
	}

    view.beginGL();
    glPushAttrib(GL_CURRENT_BIT | GL_LINE_BIT);
    glPushMatrix();
    float matrix[4][4];
    worldToDisplayViewMatrix.get(matrix);
    glMultMatrixf(&matrix[0][0]);
	glPointSize(10.0f);
    glLineWidth(10.0f);
	glColor3f(1, 1, 0);
	glBegin( GL_POINTS);
	//do the drawing here!
	 for (int v = 0; v < 100; v++) {
		 glVertex3d(0.0, 1.0, (double)v);
	 }

	 glEnd();
    glPopMatrix();
    glPopAttrib();
    view.endGL();

}

void MeshGitLocatorNode::reportError(MStatus status ){
	if(status != MStatus::kSuccess){
		MGlobal::displayInfo("ERROR " + status.errorString());
	}
}
