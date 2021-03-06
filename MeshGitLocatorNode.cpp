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
#include "EditOperation.h"

#include <maya/MPointArray.h>

#define scopeName "MeshGitLocatorNode"
MTypeId MeshGitLocatorNode::id(0x03AA4);

using namespace std;
MObject MeshGitLocatorNode::meshGitNodeConnection;

MObject MeshGitLocatorNode::drawUnmatchedObj;
MObject MeshGitLocatorNode::drawMatchedObj;
MObject MeshGitLocatorNode::drawConflictingObj;
MObject MeshGitLocatorNode::drawNonConflictingObj;

void* MeshGitLocatorNode::creator() {
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

	drawUnmatchedObj = nAttr.create("drawUnmatchedObj", "du", MFnNumericData::kBoolean,
            false, &status);
    nAttr.setKeyable(true);
    addAttribute(drawUnmatchedObj);

	drawMatchedObj = nAttr.create("drawMatchedObj", "dm", MFnNumericData::kBoolean,
            false, &status);
    nAttr.setKeyable(true);
    addAttribute(drawMatchedObj);

	drawConflictingObj = nAttr.create("drawConflictingObj", "dc", MFnNumericData::kBoolean,
            false, &status);
    nAttr.setKeyable(true);
    addAttribute(drawConflictingObj);

	drawNonConflictingObj = nAttr.create("drawNonConflictingObj", "dn", MFnNumericData::kBoolean,
            false, &status);
    nAttr.setKeyable(true);
    addAttribute(drawNonConflictingObj);

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
	
	vector<MPointArray*> verts = mgFn.getAllVerts();
	MGlobal::displayInfo("Num verts in Draw function : " + verts.size());
	//for (int g = 0; g < verts.size(); g++){
	//	for (int v = 0; v < verts[g]->length(); v++) {
	//		if(g==0){
	//			glColor3f(1, 0, 0);
	//		}
	//		else if(g==1)
	//			glColor3f(0, 1, 0);
	//		else if(g==2)
	//			glColor3f(0, 0, 1);

	//		MPoint currentV = (*verts[g])[v];
	//		//glVertex3d(currentV.x,currentV.y, currentV.z);
	//	}
	//}

		//Getting the user selected prefs for what to vizualize
    MPlug pdrawUnmatchedObj(thisObject, drawUnmatchedObj);
    bool bdrawUnmatchedObj= false;
    status = pdrawUnmatchedObj.getValue(bdrawUnmatchedObj);

	//Getting the user selected prefs for what to vizualize
    MPlug pdrawMatchedObj(thisObject, drawMatchedObj);
    bool bdrawMatchedObj= false;
    status = pdrawMatchedObj.getValue(bdrawMatchedObj);
	
	//Getting the user selected prefs for what to vizualize
    MPlug pdrawConflictingObj(thisObject, drawConflictingObj);
    bool bdrawConflictingObj= false;
    status = pdrawConflictingObj.getValue(bdrawConflictingObj);

	//Getting the user selected prefs for what to vizualize
    MPlug pdrawNonConflictingObj(thisObject, drawNonConflictingObj);
    bool bdrawNonConflictingObj= false;
    status = pdrawNonConflictingObj.getValue(bdrawNonConflictingObj);

	if(bdrawUnmatchedObj)
		drawUnmatched(mgFn);
	if(bdrawMatchedObj)
		drawMatched(mgFn);
	if(bdrawNonConflictingObj)
		drawNonconflicting(mgFn);
	if(bdrawConflictingObj){
		drawConflicting(mgFn);
	}

	glEnd();
	
	drawSelected(mgFn); 

	
    glPopMatrix();
    glPopAttrib();
    view.endGL();

}


void MeshGitLocatorNode::drawSelected(MeshGitFn &mgFn){

	MPoint original; 
	MPoint derivA; 
	MPoint derivB;
	MPoint output;

	bool isSelected = mgFn.getCurrentlySelectedEditPositions(original, derivA, derivB, output); 
	if(!isSelected)
		return;
	glPointSize(20.0f);
    glLineWidth(6.0f);
	glBegin( GL_POINTS);

	glColor3f(1, 1, 0); 
	glVertex3d(original.x , original.y, original.z);
	glVertex3d(derivA.x +dATranslateX , derivA.y +dATranslateY, derivA.z +dATranslateZ);
	glVertex3d(derivB.x +dBTranslateX, derivB.y+dBTranslateY, derivB.z+dBTranslateZ);
	glVertex3d(output.x , output.y+outputTranslateY, output.z);
	glEnd();

}

void MeshGitLocatorNode::drawUnmatched(MeshGitFn &mgFn){

	Unmatched m = mgFn.getUnmatched();

	MPointArray* dA_unmatchedPointsOrig = m.dA_unmatchedPointsOrig;
	MPointArray* dA_unmatchedPointsA = m.dA_unmatchedPointsA;
	MPointArray* dB_unmatchedPointsOrig = m.dB_unmatchedPointsOrig;
	MPointArray* dB_unmatchedPointsB = m.dB_unmatchedPointsB;

	for (int i = 0; i < dA_unmatchedPointsOrig->length(); i++) {
		glColor3f(1, 0, 0); // red
		MPoint currVert = (*dA_unmatchedPointsOrig)[i];

		glVertex3d(currVert.x - 0.01, currVert.y, currVert.z);
	}

	for (int i = 0; i < dA_unmatchedPointsA->length(); i++) {
		glColor3f(0, 1, 0); // green
		MPoint currVert = (*dA_unmatchedPointsA)[i];

		glVertex3d(currVert.x + dATranslateX, currVert.y + dATranslateY, currVert.z + dATranslateZ);
	}

	for (int i = 0; i < dB_unmatchedPointsOrig->length(); i++) {
		glColor3f(0, 0, 1); // blue
		MPoint currVert = (*dB_unmatchedPointsOrig)[i];

		glVertex3d(currVert.x + 0.01, currVert.y, currVert.z);
	}

	for (int i = 0; i < dB_unmatchedPointsB->length(); i++) {
		glColor3f(0, 1, 0); // green
		MPoint currVert = (*dB_unmatchedPointsB)[i];

		glVertex3d(currVert.x + dBTranslateX, currVert.y + dBTranslateY, currVert.z + dBTranslateZ);
	}


}

void MeshGitLocatorNode::drawMatched(MeshGitFn &mgFn) 
{
	Matched m = mgFn.getMatched();

	vector<ComponentMatch*> dA_bestMatches = m.dA_bestMatches;
	vector<ComponentMatch*> dB_bestMatches = m.dB_bestMatches;

	for (int v = 0; v < dA_bestMatches.size(); v++) {
		ComponentMatch* cM = dA_bestMatches[v];
		MPoint Ap = cM->getMatches().derivativeComp->pos;
		MPoint Op = cM->getMatches().originalComp->pos;

		glColor3f(cM->color.r, cM->color.g, cM->color.b);
		glVertex3d(Ap.x + dATranslateX, Ap.y + dATranslateY, Ap.z + dATranslateZ);
		glVertex3d(Op.x - 0.01, Op.y, Op.z);
	}

	for (int i = 0; i < dB_bestMatches.size(); i++) {
		ComponentMatch* cM = dB_bestMatches[i];
		MPoint Bp = cM->getMatches().derivativeComp->pos;
		MPoint Op = cM->getMatches().originalComp->pos;

		glColor3f(cM->color.r, cM->color.g, cM->color.b);
		glVertex3d(Bp.x + dBTranslateX, Bp.y + dBTranslateY, Bp.z + dBTranslateZ);
		glVertex3d(Op.x + 0.01, Op.y, Op.z);
	}
}


void MeshGitLocatorNode::drawNonconflicting(MeshGitFn &mgFn)
{
	vector<EditOperation*> eOs = mgFn.getNonconflictingEdits();

	for (int i = 0; i < eOs.size(); i++) {

		EditOperation* eO = eOs[i];

		if (eO->aChanged) {
			ComponentMatch* cM = eO->matchA;
			MPoint Op = cM->getMatches().derivativeComp->pos;

			glColor3f(0,1,0);
			glVertex3d(Op.x+ dATranslateX, Op.y+ dATranslateY, Op.z+ dATranslateZ);
		}

		if (eO->bChanged) {
			ComponentMatch* cM = eO->matchB;
			MPoint Op = cM->getMatches().derivativeComp->pos;

			glColor3f(0,1,0);
			glVertex3d(Op.x+ dBTranslateX, Op.y+ dBTranslateY, Op.z+ dBTranslateZ);
		}
	}
}

void MeshGitLocatorNode::drawConflicting(MeshGitFn &mgFn)
{
	vector<EditOperation*> eOs = mgFn.getConflictingEdits();

	for (int i = 0; i < eOs.size(); i++) {

		EditOperation* eO = eOs[i];

		if (eO->aChanged) {
			ComponentMatch* cM = eO->matchA;
			MPoint Op = cM->getMatches().derivativeComp->pos;

			glColor3f(1,0,0);
			glVertex3d(Op.x+ dATranslateX, Op.y+ dATranslateY, Op.z+ dATranslateZ);
		}

		if (eO->bChanged) {
			ComponentMatch* cM = eO->matchB;
			MPoint Op = cM->getMatches().derivativeComp->pos;

			glColor3f(1,0,0);
			glVertex3d(Op.x+ dBTranslateX, Op.y+ dBTranslateY, Op.z+ dBTranslateZ);
		}
	}
}

void MeshGitLocatorNode::reportError(MStatus status ){
	if(status != MStatus::kSuccess){
		MGlobal::displayInfo("ERROR " + status.errorString());
	}
}
