#define MNoVersionString
#define MNoPluginEntry
#include "MeshGitNode.h"
#include "MatchComputer.h"
#include <maya/MGlobal.h>
#include <maya/MString.h>
#include "maya/MFnPlugin.h"
#include <maya/MFnStringData.h>
#include <maya/MPlugArray.h>
#include <maya/MDagPath.h>
#include <maya/MFnStringArrayData.h>
#include <string>
#include <iostream>
using namespace std;

#define McheckErr(stat,msg)			\
	if ( MS::kSuccess != stat ) {	\
		cerr << msg;				\
		return MS::kFailure;		\
	}

MStatus returnStatus;

//IMPLEMENTATION

//Node Attributes
MObject MeshGitNode::originalMesh;
MObject MeshGitNode::derivativeAMesh;
MObject MeshGitNode::derivativeBMesh;
MObject MeshGitNode::mergedMesh;
MObject MeshGitNode::currentFrame;
MObject MeshGitNode::editStringArray;

MTypeId MeshGitNode::id( 0x4E5000 );

void* MeshGitNode::creator()
{
	return new MeshGitNode;
}

MeshGitNode::MeshGitNode(void)
{
}


MeshGitNode::~MeshGitNode(void)
{
}

MStatus MeshGitNode::initialize()
{
	MGlobal::displayInfo("init started");
	MFnUnitAttribute unitAttr;
	MFnTypedAttribute typedAttr;
	MFnNumericAttribute numericAttr;
	MFnStringData stringFn;
	MFnStringArrayData stringArrayFn;
	MStatus returnStatus;

	//Create the attributes
	// Current time
    //
    MeshGitNode::currentFrame = unitAttr.create("currentFrame", "ct", MFnUnitAttribute::kTime, 0,
            &returnStatus);
    unitAttr.setKeyable(true);
    //uAttr.setHidden(true);
    returnStatus = addAttribute(MeshGitNode::currentFrame);
	McheckErr(returnStatus, "ERROR adding currentFrame attribute\n");

	MeshGitNode::editStringArray = typedAttr.create( "editStringArray", "es",
		MFnData::kStringArray,
												 &returnStatus ); 
	McheckErr(returnStatus, "ERROR creating MeshGitNode editStringArray attribute\n");
	typedAttr.setStorable(true);
	returnStatus = addAttribute(MeshGitNode::editStringArray);
	McheckErr(returnStatus, "ERROR adding editStringArray attribute\n");

	MeshGitNode::originalMesh = typedAttr.create( "originalMesh", "om",
												 MFnData::kMesh,
												 &returnStatus ); 
	McheckErr(returnStatus, "ERROR creating MeshGitNode output attribute\n");
	typedAttr.setStorable(false);
	returnStatus = addAttribute(MeshGitNode::originalMesh);
	McheckErr(returnStatus, "ERROR adding originalMesh attribute\n");

	MeshGitNode::derivativeAMesh = typedAttr.create( "derivativeAMesh", "dam",
												 MFnData::kMesh,
												 &returnStatus ); 
	McheckErr(returnStatus, "ERROR creating MeshGitNode output attribute\n");
	typedAttr.setStorable(false);
	returnStatus = addAttribute(MeshGitNode::derivativeAMesh);
	McheckErr(returnStatus, "ERROR adding derivativeAMesh attribute\n");

	MeshGitNode::derivativeBMesh = typedAttr.create( "derivativeBMesh", "dbm",
												 MFnData::kMesh,
												 &returnStatus ); 
	McheckErr(returnStatus, "ERROR creating MeshGitNode output attribute\n");
	typedAttr.setStorable(false);
	returnStatus = addAttribute(MeshGitNode::derivativeBMesh);
	McheckErr(returnStatus, "ERROR adding derivativeBMesh attribute\n");

	MeshGitNode::mergedMesh = typedAttr.create( "mergedMesh", "mm",
												 MFnData::kMesh,
												 &returnStatus ); 
	McheckErr(returnStatus, "ERROR creating MeshGitNode output attribute\n");
	typedAttr.setStorable(false);
	returnStatus = addAttribute(MeshGitNode::mergedMesh);
	McheckErr(returnStatus, "ERROR adding mergedMesh attribute\n");



	//Set the attribute affects
	returnStatus = attributeAffects(currentFrame, outputGeom);
	McheckErr(returnStatus, "AttributeAffects error for 'currentFrame'");
	returnStatus = attributeAffects(MeshGitNode::originalMesh, outputGeom);
	McheckErr(returnStatus, "AttributeAffects error for 'blendEndFrames'");
	returnStatus = attributeAffects(MeshGitNode::derivativeAMesh, outputGeom);
	McheckErr(returnStatus, "AttributeAffects error for 'blendEndFrames'");
	returnStatus = attributeAffects(MeshGitNode::derivativeBMesh, outputGeom);
	McheckErr(returnStatus, "AttributeAffects error for 'blendEndFrames'");

	returnStatus = attributeAffects(inputGeom, outputGeom);
	McheckErr(returnStatus, "AttributeAffects error for 'currentFrame'");

	//std::cout << "Initialize Completed" << std::endl;
	MGlobal::displayInfo("Initialize Completed!!!!");

	

	return MS::kSuccess;
}

void MeshGitNode::postConstructor() {
	cout<<"POST CONSTRUCTOR CALLED" << endl;
	mergedVerts = new MPointArray(); 
	diffResultsWindowName = "none";
	diffResultsScrollListName = "none";
	selectedEditIndex=-1; 
}

MStatus MeshGitNode::storeAllVerts(MDataBlock& dataBlock)
{
	MStatus status;
	MGlobal::displayInfo("Starting storeAllVerts");

	//Gets the handle to the output geometry
	MArrayDataHandle ouputGeomHandle = dataBlock.outputArrayValue(outputGeom, &status);
	reportError(status);

	//Gets the number of geometries on the handle - should be 3
	unsigned int numGeom = ouputGeomHandle.elementCount(&status);
	reportError(status);

	allVerts.clear();
	allVerts.resize(numGeom);
	allTransforms.clear();
	allTransforms.resize(numGeom);
	allMFnMeshObjects.clear();
	allMFnMeshObjects.resize(numGeom);

	//Get the input handle
	MArrayDataHandle inputHandle = dataBlock.outputArrayValue(input, &status);
	reportError(status);
	//cout<< "NUM GEOMETRIES: " << numGeom << endl;

	for(unsigned int i=0; i<numGeom; i++){
		//MGlobal::displayInfo("Storing Geom " + i);
		//get the element index that the current geom is in the datablock
		int geomIndex = ouputGeomHandle.elementIndex(&status);

		//go tot he correct geometry in the input geometry handle
		status = inputHandle.jumpToElement(geomIndex);
		reportError(status);

		////get the value of the geometry
		MDataHandle inputGeomValue = inputHandle.inputValue(&status);
		reportError(status);
		
		MObject meshObject = inputGeomValue.asMesh();
		MFnMesh *currentMeshFn = new MFnMesh(meshObject);
		allMFnMeshObjects[i] = currentMeshFn;
		//cout<< " NUM POLYGONS " << allMFnMeshObjects[i]->numPolygons() << endl;

		//Get the input geometry handle, and the group id as well 
		MDataHandle hGeom = inputGeomValue.child(inputGeom);
        MDataHandle hGroup = inputGeomValue.child(groupId);
        unsigned int groupId = hGroup.asLong();
		if(i == 3)
			mergedMeshGroupID= groupId;


		////Get the output geometry handle output value
		MDataHandle hOutputGeomValue = ouputGeomHandle.outputValue(&status);

		////Copy over the data rfom the input to the output
		status = hOutputGeomValue.copy(hGeom);

		MDataHandle forMat = ouputGeomHandle.inputValue();
		//MMatrix mat = forMat.geometryTransformMatrix();
		////cout<< allTransforms[0] <<endl;
		//allTransforms.push_back(mat);

		////set an iterator over all points in the current input geometry and copy all the points into the all verts data structure
		MItGeometry iter(hOutputGeomValue, groupId, false);
		MPointArray currentVerts; 
		iter.allPositions(currentVerts, MSpace::kObject);
		ouputGeomHandle.next();
		//cout << "MESH " << i << endl;
		//cout<< currentVerts << endl;
		MPointArray* mp = new MPointArray(currentVerts);
		allVerts[i] = mp;
	}

	ouputGeomHandle.setAllClean();
	MGlobal::displayInfo("Ending storeAllVerts");

	//for (unsigned int i = 0; i < allVerts.size(); i++) {
		//cout << "MESHB " << i << endl;
		//cout<< *allVerts[i] << endl;
		//cout<<allTransforms[i]<<endl;
	//}
	return status;
}





vector<MPointArray*> MeshGitNode::getAllVerts() 
{
	return allVerts;
}

vector<MFnMesh*> MeshGitNode::getMFnMeshObjects() 
{
	return allMFnMeshObjects;
}

MStatus MeshGitNode::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	MStatus status = MStatus::kSuccess;
	//if(allVerts.length()>3000)
	//	allVerts.clear();

	MGlobal::displayInfo("COMPUTE CALLED!!"); 
    if(plug.attribute() == outputGeom) {
		MGlobal::displayInfo("plug is outputgeom "); 
		storeAllVerts(dataBlock);
		status = MStatus::kSuccess;
		status = deformOutputMesh(dataBlock);

		//

    }

	MGlobal::displayInfo("Num geometries at end of COMPUTE function : " + allVerts.size());

	return status;
}

MStatus MeshGitNode::deformOutputMesh(MDataBlock &dataBlock) {
	//cout<<"Starting deforming merged mesh" << endl;
	MStatus status;
	if(mergedVerts==NULL){
		//cout<<"NULL MERGEDVERTS" << endl;
	}
	//cout<<"MERGEDVERTS NOT NULL" << endl;

	if(mergedVerts->length()<=1){
		//cout<<"MERGEDVERTS NO LENGTH" << endl;
	}


	if(mergedVerts==NULL || mergedVerts->length()<=1){
		//cout<<"Merged verts empty or null" << endl;
		return status; 
	}
	
    
	//cout<<"Starting deforming merged mesh 2" << endl;
    MArrayDataHandle outputGeometryArrayHandle = dataBlock.outputArrayValue(outputGeom,
            &status);

    int numOutputGeoms= outputGeometryArrayHandle.elementCount(&status);
   

    for (unsigned g = 0; g < numOutputGeoms; ++g) {
		if(g<3){ 
			 outputGeometryArrayHandle.next();
			continue; 
		}

        MDataHandle hOutputGeom = outputGeometryArrayHandle.outputValue(&status);

        //Transform to local space
        unsigned int plugIndex = outputGeometryArrayHandle.elementIndex(&status);
        MPlug outputGeometryPlug(thisMObject(), outputGeom);
        MPlug outputPlug = outputGeometryPlug.elementByLogicalIndex(plugIndex,
                &status);

        // Get the connected node to have access to the path data
        // of the actual geometry
        MPlugArray connectedPlugs;
        outputPlug.connectedTo(connectedPlugs, true, true);
        MObject obj;
        MDagPath dagPath;

        // Iterate over each point in the output: by setting the group id, will
        // Only get the points in the deformation set
        MItGeometry iter(hOutputGeom, mergedMeshGroupID, false);
        // Set the position on the iterator(will also set on output as point
        // to same thing
		status = iter.setAllPositions(*mergedVerts, MSpace::kObject);

        // go to next geometry
        outputGeometryArrayHandle.next();
    }

	findSelectedEditIndex(); 
	//cout<<"Finished deforming merged mesh" << endl;
    return status;
} // setupOutputGeometry

void MeshGitNode::startDiff()
{
	mergedVerts = allVerts[0]; 
	MPointArray* meshVertsB = allVerts[2];
	cout << "meshVertsAXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" << endl;
	cout<< *meshVertsB << endl;
	meshOperator = new MeshOperator(allVerts, allMFnMeshObjects);
	meshOperator->diff();
	
	//MPointArray* meshVertsOrig = allVerts[0];
	//MPointArray* meshVertsA = allVerts[1];
	//MPointArray* meshVertsB = allVerts[2];
	//MFnMesh* fnOrig = allMFnMeshObjects[0];
	//MFnMesh* fnA = allMFnMeshObjects[1];
	//MFnMesh* fnB = allMFnMeshObjects[2];

	///*for (unsigned int i = 0; i < meshVertsOrig.length(); i++) {
	//	cout << meshVertsOrig[i] << endl;
	//}*/

	//MatchComputer* matchComputerA = new MatchComputer(meshVertsOrig, meshVertsA, fnOrig,fnA );
	//MatchComputer* matchComputerB = new MatchComputer(meshVertsOrig, meshVertsB, fnOrig,fnB );

	dA_bestMatches = meshOperator->dA_bestMatches;
	dA_unmatchedPointsOrig = meshOperator->dA_unmatchedPointsOrig;
	dA_unmatchedPointsA = meshOperator->dA_unmatchedPointsA;

	dB_bestMatches = meshOperator->dB_bestMatches;
	dB_unmatchedPointsOrig = meshOperator->dB_unmatchedPointsOrig;
	dB_unmatchedPointsB = meshOperator->dB_unmatchedPointsB;

	meshOperator->checkConflicts();
	nonconflictingOriginalVerts = meshOperator->nonconflictingEdits;
	conflictingOriginalVerts = meshOperator->conflictingEdits;
	vector<MString> editStrings = meshOperator->editInfo;
	//Present list of edits 
	if(diffResultsWindowName=="none"){
		diffResultsWindowName = MGlobal::executeCommandStringResult("window -title \"DIFF RESULTS\" -widthHeight 600 400; columnLayout;");
		MGlobal::executeCommand("text -label \"Edit Operation Results\";");
		MString command2 = "textScrollList -numberOfRows 20 -allowMultiSelection false -width 550 -selectCommand (\"updateSelectedEditIndex()\") ";
		for(int i = 0; i < editStrings.size(); i++){
			MString s = editStrings.at(i);
			command2 += "-append ";
			command2 += "\"";
			command2 += s;
			command2 += "\" ";
		}
		command2 += " ;";
		diffResultsScrollListName = MGlobal::executeCommandStringResult(command2);
		MGlobal::executeCommand("button -label \"Auto Merge NonConflicting Edits\" -command \" \";");
		MGlobal::executeCommand("button -label \"Manually Merge Selected Conflict\" -command \" \";");
		MGlobal::executeCommand("showWindow;");
		cout << "diffResultsWindowName " << diffResultsWindowName << endl;
		cout << "diffResultsScrollListName " << diffResultsScrollListName << endl;
	}
	else 
		reloadDiffResultsWindow(); 
}

void  MeshGitNode::reloadDiffResultsWindow(){
	meshOperator->updateEditStrings(); 
	cout<<"Starting reload Diff" << endl; 
	MGlobal::executeCommand("textScrollList -e -removeAll " + diffResultsScrollListName); 

	vector<MString> editStrings = meshOperator->editInfo;
	for(int i = 0; i < editStrings.size(); i++){
		MString command2 = "textScrollList -e -append "; 
		MString s = editStrings.at(i);
		command2 += "\"";
		command2 += s;
		command2 += "\" ";
		MString finalCommand = command2 + " " + diffResultsScrollListName;
		MGlobal::executeCommand(finalCommand); 
		cout<<"Text Scroll Command " << finalCommand << endl; 
	}//window1|columnLayout114|textScrollList21
	
}

int MeshGitNode::findSelectedEditIndex(){
	if(diffResultsScrollListName=="none")
		return 0; 

	//cout<<"Starting find selected edit index" << endl; 
	MString finalCommand = "textScrollList -q -selectIndexedItem " + diffResultsScrollListName;
	MIntArray indexArray; 
	MGlobal::executeCommand(finalCommand, indexArray); 
	cout<<"indexArray.length " << indexArray.length() <<endl;
	if(indexArray.length()<=0)
		return 0; 
	selectedEditIndex = indexArray[0]-1; 
	cout<<"selectedEditIndex " << selectedEditIndex<<endl;

	if (selectedEditIndex != -1) { // check valid index
		bool conflicting = meshOperator->conflictingEdit(selectedEditIndex);
		if (conflicting) {
			MGlobal::executeCommand("if (`window -exists ResolveConflictGUI`) { deleteUI -window ResolveConflictGUI; }");
			MGlobal::executeCommand("window -title \"Resolve Conflict\" -w 100 -h 150 ResolveConflictGUI; columnLayout -adjustableColumn true;");
			MGlobal::executeCommand("button -label \"Resolve with A\" -command conflictResolveA -actOnPress false;");
			MGlobal::executeCommand("button -label \"Resolve with B\" -command conflictResolveB -actOnPress false;");
			MGlobal::executeCommand("button -label \"Resolve both\" -command conflictResolveBoth -actOnPress false;");
			MGlobal::executeCommand("setParent..; showWindow ResolveConflictGUI;");
		} 
		else {
			MGlobal::executeCommand("if (`window -exists ResolveConflictGUI`) { deleteUI -window ResolveConflictGUI; }");
		}
	}

	return selectedEditIndex; 
	
}

void MeshGitNode::manualResolveConflict(int rc)
{
	mergedVerts = meshOperator->manualResolveEdit(rc);
	reloadDiffResultsWindow();
}

bool MeshGitNode::getCurrentlySelectedEditPositions(MPoint& orginal, MPoint & derivA, MPoint &derivB, MPoint &output){
	if(selectedEditIndex==-1)
		return false; 

	//cout<<"Starting find selected edit index" << endl; 
	vector<EditOperation*> allEdits = meshOperator->allEdits;
	if(allEdits.size()<=selectedEditIndex){
		return false;
	}
	
	EditOperation* edit = allEdits[selectedEditIndex];
	ComponentMatch* matchA = edit->matchA;
	Match A = matchA->getMatches();
	MeshComponent* A_original = A.originalComp;
	MeshComponent* A_derivative = A.derivativeComp;


	ComponentMatch* matchB = edit->matchB;
	Match B = matchB->getMatches();
	MeshComponent* B_original = B.originalComp;
	MeshComponent* B_derivative = B.derivativeComp;

	
	orginal = A_original->pos;
	derivA = A_derivative->pos;
	derivB = B_derivative->pos;
   int index = A_original->index;
	if(mergedVerts->length()<=index)
		return true;
	output = (*mergedVerts)[index]; 
	cout<<"Output Vert " << output<< endl; 

	return true;
}
void MeshGitNode::mergeUnconflicting(){

	mergedVerts = meshOperator->mergeUnconflictingEdits(); 
	
	reloadDiffResultsWindow(); 
	//need to set something dirty so that it recomputes
}

vector<MString> MeshGitNode::getEditStrings( ){
	if(meshOperator==NULL)
		return vector<MString>();

	else return
		meshOperator->editInfo;
}


//PRINTING AND DEBUGGING FUNCTIONS
void MeshGitNode::reportError(MStatus status )
{
	if(status != MStatus::kSuccess){
		MGlobal::displayInfo("ERROR in MeshGitDeformerNode " + status.errorString());
	}
}
void MeshGitNode::printTEST()
{
	MGlobal::displayInfo("TESTINGGGG");

}

void MeshGitNode::printVectorOfPoints(MString name, std::vector<MPointArray> &points)
{
	MGlobal::displayInfo(name);
	for(int i = 0; i <points.size(); i ++){
		for(int j = 0; j <points[i].length(); j ++){
			double x = points[i][j].x;
			double y = points[i][j].y;
			double z = points[i][j].z;
			//std::string toPrint = "g" + i + std::string("v") + j + std::string("[") + x + std::string(",") + y + std::string(",") + z + std::string("]");
			MString toPrint2 = "g" + i ;//+ "v" + j + "[" + x + "," + y + "," + z + "]";
			toPrint2 += "v" + j;
			toPrint2 +="[" + (int)x;
			toPrint2 +="," +(int) y ;
			toPrint2 +="," +(int) z ;
			toPrint2 +="]";
			//MGlobal::displayInfo("POINT");
			MGlobal::displayInfo(toPrint2);
		}
	}

}


MStatus
MeshGitNode::deform( MDataBlock& block,
				MItGeometry& iter,
				unsigned int multiIndex)
//
// Method: deform
//
// Description:   Deform the point with a squash algorithm
//
// Arguments:
//   block		: the datablock of the node
//	 iter		: an iterator for the geometry to be deformed
//	 multiIndex : the index of the geometry that we are deforming
//
//
{
	MGlobal::displayInfo("Deformation Started: num geoms = " + allVerts.size());
	MStatus returnStatus;
	
	// Envelope data from the base class.
	// The envelope is simply a scale factor.
	//
	MDataHandle envData = block.inputValue(envelope, &returnStatus);
	if (MS::kSuccess != returnStatus) return returnStatus;
	float env = envData.asFloat();	


	// iterate through each point in the geometry
	//
	//if(multiIndex==0 ||allVerts.length()>10000){
	//	allVerts.clear();
	//}
	//for ( ; !iter.isDone(); iter.next()) {
	//	MPoint pt = iter.position();
	//	//pt *= omatinv;
	//	allVerts.append(pt.x,pt.y,pt.z,1.0);
	//	//float weight = weightValue(block,multiIndex,iter.index());
	//	
	//	// offset algorithm
	//	//
	//	//pt.y = pt.y + env*weight;
	//	//
	//	// end of offset algorithm

	//	//pt *= omat
	//	//MPoint pt = 
	//	pt = pt;
	//	iter.setPosition(pt);
	//}
	//MGlobal::displayInfo("Num verts in node function : " + allVerts.length());

	return returnStatus;
}