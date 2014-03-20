#define MNoVersionString
#define MNoPluginEntry
#include "MeshGitNode.h"
#include <maya/MGlobal.h>
#include <maya/MString.h>
#include "maya/MFnPlugin.h"
#include <maya/MFnStringData.h>


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
	MGlobal::displayInfo("Deformation Started: " + allVerts.length());
	MStatus returnStatus;
	
	// Envelope data from the base class.
	// The envelope is simply a scale factor.
	//
	MDataHandle envData = block.inputValue(envelope, &returnStatus);
	if (MS::kSuccess != returnStatus) return returnStatus;
	float env = envData.asFloat();	


	// iterate through each point in the geometry
	//
	if(multiIndex==0 ||allVerts.length()>10000){
		allVerts.clear();
	}
	for ( ; !iter.isDone(); iter.next()) {
		MPoint pt = iter.position();
		//pt *= omatinv;
		allVerts.append(pt.x,pt.y,pt.z,1.0);
		//float weight = weightValue(block,multiIndex,iter.index());
		
		// offset algorithm
		//
		//pt.y = pt.y + env*weight;
		//
		// end of offset algorithm

		//pt *= omat
		//MPoint pt = 
		pt = pt;
		iter.setPosition(pt);
	}
	MGlobal::displayInfo("Num verts in node function : " + allVerts.length());

	return returnStatus;
}


void MeshGitNode::getAllVerts(MPointArray &verts){
	verts = allVerts;

}

void MeshGitNode::printTEST(){
	MGlobal::displayInfo("TESTINGGGG");

}

MStatus MeshGitNode::compute(const MPlug& plug, MDataBlock& dataBlock){
	MStatus status = MStatus::kSuccess;
	if(allVerts.length()>3000)
		allVerts.clear();
	MGlobal::displayInfo("COMPUTE CALLED!!"); 
        if (plug.attribute() == outputGeom) {
			
			MGlobal::displayInfo("plug is outputgeom "); 
                // get the input corresponding to this output
                //
                unsigned int index = plug.logicalIndex();
                MObject thisNode = this->thisMObject();
                MPlug inPlug(thisNode,input);
                inPlug.selectAncestorLogicalIndex(index,input);
                MDataHandle hInput = dataBlock.inputValue(inPlug);

                // get the input geometry and input groupId
                //
                MDataHandle hGeom = hInput.child(inputGeom);
                MDataHandle hGroup = hInput.child(groupId);
                unsigned int groupId = hGroup.asLong();
                MDataHandle hOutput = dataBlock.outputValue(plug);
                hOutput.copy(hGeom);

                // do the deformation
                //
                MItGeometry iter(hOutput,groupId,false);
                for ( ; !iter.isDone(); iter.next()) {
                        MPoint pt = iter.position();
						allVerts.append(pt.x,pt.y,pt.z,1.0);
                        //
            // insert deformation code here
                        //

                        iter.setPosition(pt);
                }
                status = MStatus::kSuccess;
        }
		MGlobal::displayInfo("Num verts in COMPUTE function : " + allVerts.length());
	return status;
}