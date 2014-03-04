#define MNoVersionString
#define MNoPluginEntry
#include "MeshGitNode.h"
#include <maya/MGlobal.h>

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
	returnStatus = attributeAffects(MeshGitNode::originalMesh, MeshGitNode::mergedMesh);
	McheckErr(returnStatus, "AttributeAffects error for 'blendEndFrames'");
	returnStatus = attributeAffects(MeshGitNode::derivativeAMesh, MeshGitNode::mergedMesh);
	McheckErr(returnStatus, "AttributeAffects error for 'blendEndFrames'");
	returnStatus = attributeAffects(MeshGitNode::derivativeBMesh, MeshGitNode::mergedMesh);
	McheckErr(returnStatus, "AttributeAffects error for 'blendEndFrames'");

	std::cout << "Initialize Completed" << std::endl;
	MGlobal::displayInfo("Initialize Completed");

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
	MStatus returnStatus;
	
	// Envelope data from the base class.
	// The envelope is simply a scale factor.
	//
	MDataHandle envData = block.inputValue(envelope, &returnStatus);
	if (MS::kSuccess != returnStatus) return returnStatus;
	float env = envData.asFloat();	


	// iterate through each point in the geometry
	//
	for ( ; !iter.isDone(); iter.next()) {
		MPoint pt = iter.position();
		//pt *= omatinv;
		
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
	return returnStatus;
}


