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

#include <maya/MPointArray.h>

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
		return;
	}
	
	MObject meshGitNodeObject = connectedElements[0].node();
	MGlobal::displayInfo("Got meshgitnode object :"+ connectedElements[0].name());
	//Attach a fnset to the object so that we can access what is underneath
	MeshGitFn mgFn;
	status = mgFn.setObject(meshGitNodeObject);
	reportError(status);
	MGlobal::displayInfo("Attached fnsetobject to the nodeobject");
	

    view.beginGL();
    glPushAttrib(GL_CURRENT_BIT | GL_LINE_BIT);
    glPushMatrix();
    float matrix[4][4];
    worldToDisplayViewMatrix.get(matrix);
    glMultMatrixf(&matrix[0][0]);
	glPointSize(10.0f);
    glLineWidth(6.0f);
	glColor3f(0, 1, 0);
	glBegin( GL_POINTS);
	//do the drawing here!

	 //for (int v = 0; v < 100; v++) {
		// glVertex3d(0.0, 1.0, (double)v);
	 //}
	
	std::vector<MPointArray> verts;
	mgFn.getAllVerts(verts);
	MGlobal::displayInfo("Num verts in Draw function : " + verts.size());
	for (int g = 0; g < verts.size(); g++){
		for (int v = 0; v < verts[g].length(); v++) {
			if(g==0){
				glColor3f(1, 0, 0);
			}
			else if(g==1)
				glColor3f(0, 1, 0);
			else if(g==2)
				glColor3f(0, 0, 1);

			MPoint currentV = verts[g][v];
			//glVertex3d(currentV.x,currentV.y, currentV.z);
		}
	}

	drawUnmatched(mgFn);
	drawMatched(mgFn);

	glEnd();
    glPopMatrix();
    glPopAttrib();
    view.endGL();

}


void MeshGitLocatorNode::drawUnmatched(MeshGitFn &mgFn){
	MPointArray unmatchedOriginalMeshPoints;
	MPointArray unmatchedDerivativeMeshPoints;
	mgFn.getUnmatchedOriginalMeshPoints(unmatchedOriginalMeshPoints);
	mgFn.getUnmatchedDerivativeMeshPoints(unmatchedDerivativeMeshPoints);
	for (int v = 0; v < unmatchedOriginalMeshPoints.length(); v++) {
			glColor3f(1, 0, 0);
			MPoint currentV = unmatchedOriginalMeshPoints[v];
			glVertex3d(currentV.x,currentV.y, currentV.z);
	}
	for (int v = 0; v < unmatchedDerivativeMeshPoints.length(); v++) {
			glColor3f(0, 1, 0);
			MPoint currentV = unmatchedDerivativeMeshPoints[v];
			glVertex3d(currentV.x+derivativeATranslateX,currentV.y+derivativeATranslateY, currentV.z+derivativeATranslateZ);
	}


}

void MeshGitLocatorNode::drawMatched(MeshGitFn &mgFn){
	std::vector<ComponentMatch> bestComponentMatches;
	mgFn.getBestComponentMatches(bestComponentMatches);
	for (int v = 0; v < bestComponentMatches.size(); v++) {
			
		ComponentMatch cM = bestComponentMatches[v];
		MPoint aP = cM.getMatches().derivativeComp.pos;
		MPoint bP = cM.getMatches().originalComp.pos;

		glColor3f(cM.color.r, cM.color.g, cM.color.b);
		glVertex3d(aP.x+derivativeATranslateX, aP.y+derivativeATranslateY, aP.z+derivativeATranslateZ);
		glVertex3d(bP.x, bP.y, bP.z);
	}


}


void MeshGitLocatorNode::reportError(MStatus status ){
	if(status != MStatus::kSuccess){
		MGlobal::displayInfo("ERROR " + status.errorString());
	}
}
