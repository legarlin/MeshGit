#define MNoVersionString
#define MNoPluginEntry
#include "MeshGitNode.h"
#include "MatchComputer.h"
#include <maya/MGlobal.h>
#include <maya/MString.h>
#include "maya/MFnPlugin.h"
#include <maya/MFnStringData.h>
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

MStatus MeshGitNode::storeAllVerts(MDataBlock& dataBlock){
	MStatus status;
		MGlobal::displayInfo("Starting storeAllVerts");

	    //Gets the handle to the output geometry
	    MArrayDataHandle ouputGeomHandle = dataBlock.outputArrayValue(outputGeom,
            &status);
		reportError(status);

		//Gets the number of geometries on the handle - should be 3
		unsigned int numGeom = ouputGeomHandle.elementCount(&status);
		reportError(status);
		allVerts.clear();
		allVerts.resize(numGeom);
		
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
			
			//Get the input geometry handle, and the group id as well 
			MDataHandle hGeom = inputGeomValue.child(inputGeom);
            MDataHandle hGroup = inputGeomValue.child(groupId);
            unsigned int groupId = hGroup.asLong();

			////Get the output geometry handle output value
			MDataHandle hOutputGeomValue = ouputGeomHandle.outputValue(&status);

			////Copy over the data rfom the input to the output
			status = hOutputGeomValue.copy(hGeom);

			////set an iterator over all points in the current input geometry and copy all the points into the all verts data structure
			MItGeometry iter(hOutputGeomValue, groupId, false);
			allVerts[i].clear();
			iter.allPositions(allVerts[i], MSpace::kWorld);
			ouputGeomHandle.next();

		}

		ouputGeomHandle.setAllClean();
		MGlobal::displayInfo("Ending storeAllVerts");
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


void MeshGitNode::getAllVerts(std::vector<MPointArray> &verts){
	verts = allVerts;

}



MStatus MeshGitNode::compute(const MPlug& plug, MDataBlock& dataBlock){
	MStatus status = MStatus::kSuccess;
	//if(allVerts.length()>3000)
	//	allVerts.clear();

	MGlobal::displayInfo("COMPUTE CALLED!!"); 
        if (plug.attribute() == outputGeom) {
			
			MGlobal::displayInfo("plug is outputgeom "); 

			storeAllVerts(dataBlock);
                // get the input corresponding to this output
                //
                //unsigned int index = plug.logicalIndex();
                //MObject thisNode = this->thisMObject();
                //MPlug inPlug(thisNode,input);
                //inPlug.selectAncestorLogicalIndex(index,input);
                //MDataHandle hInput = dataBlock.inputValue(inPlug);

                //// get the input geometry and input groupId
                ////
                //MDataHandle hGeom = hInput.child(inputGeom);
                //MDataHandle hGroup = hInput.child(groupId);
                //unsigned int groupId = hGroup.asLong();
                //MDataHandle hOutput = dataBlock.outputValue(plug);
                //hOutput.copy(hGeom);

                // do the deformation
                //
      //          MItGeometry iter(hOutput,groupId,false);
      //          for ( ; !iter.isDone(); iter.next()) {
      //                  MPoint pt = iter.position();
						//allVerts.append(pt.x,pt.y,pt.z,1.0);
      //                  //
      //      // insert deformation code here
      //                  //

      //                  iter.setPosition(pt);
      //          }
                status = MStatus::kSuccess;
        }

		MGlobal::displayInfo("Num geometries at end of COMPUTE function : " + allVerts.size());

	return status;
}

void MeshGitNode::startDiff(){
	MPointArray meshVerts1 = allVerts[0];
	MPointArray meshVerts2 = allVerts[1];
		for (unsigned int i = 0; i < meshVerts1.length(); i++) {
			cout<<meshVerts1[i]<< endl;
		}
	MatchComputer matchComputer(allVerts[0],allVerts[1]);

}
//PRINTING AND DEBUGGING FUNCTIONS

void MeshGitNode::reportError(MStatus status ){
	if(status != MStatus::kSuccess){
		MGlobal::displayInfo("ERROR in MeshGitDeformerNode " + status.errorString());
	}
}
void MeshGitNode::printTEST(){
	MGlobal::displayInfo("TESTINGGGG");

}

void MeshGitNode::printVectorOfPoints(MString name, std::vector<MPointArray> &points){
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
