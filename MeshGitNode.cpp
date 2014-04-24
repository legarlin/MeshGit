#define MNoVersionString
#define MNoPluginEntry
#include "MeshGitNode.h"
#include "MatchComputer.h"
#include <maya/MGlobal.h>
#include <maya/MString.h>
#include "maya/MFnPlugin.h"
#include <maya/MFnStringData.h>
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

	for(unsigned int i=0; i<numGeom; i++){
		MGlobal::displayInfo("Storing Geom " + i);
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
		cout << "MESH " << i << endl;
		cout<< currentVerts << endl;
		MPointArray* mp = new MPointArray(currentVerts);
		allVerts[i] = mp;
	}

	ouputGeomHandle.setAllClean();
	MGlobal::displayInfo("Ending storeAllVerts");

	for (unsigned int i = 0; i < allVerts.size(); i++) {
		cout << "MESHB " << i << endl;
		cout<< *allVerts[i] << endl;
		cout<<allTransforms[i]<<endl;
	}
	return status;
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
    }

	MGlobal::displayInfo("Num geometries at end of COMPUTE function : " + allVerts.size());

	return status;
}

void MeshGitNode::startDiff()
{
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

	vector<MString> editStrings = meshOperator->editInfo;
	//Present list of edits 
	
	MGlobal::executeCommand("window -title \"DIFF RESULTS\" -widthHeight 600 400; columnLayout;");
	MGlobal::executeCommand("text -label \"Edit Operation Results\";");
	MString command2 = "textScrollList -numberOfRows 20 -allowMultiSelection false -width 550";
	for(int i = 0; i < editStrings.size(); i++){
		MString s = editStrings.at(i);
		command2 += "-append ";
		command2 += "\"";
		command2 += s;
		command2 += "\" ";
	}
	command2 += " ;";
	MGlobal::executeCommand(command2);
	MGlobal::executeCommand("button -label \"Auto Merge NonConflicting Edits\" -command \" \";");
	MGlobal::executeCommand("button -label \"Manually Merge Selected Conflict\" -command \" \";");
	MGlobal::executeCommand("showWindow;");
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
