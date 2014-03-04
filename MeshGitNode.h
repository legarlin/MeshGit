#include <maya/MTime.h>
#include <maya/MFnMesh.h>
#include <maya/MPoint.h>
#include <maya/MFloatPoint.h>
#include <maya/MFloatPointArray.h>
#include <maya/MIntArray.h>
#include <maya/MDoubleArray.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnNumericAttribute.h>
//#include <maya/MFnPlugin.h>

#include <maya/MPxNode.h>
#include <maya/MPxDeformerNode.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MFnMeshData.h>
#include <maya/MIOStream.h>
#include <maya/MItGeometry.h>

//HEADER
class MeshGitNode : public MPxDeformerNode
{
public:
					MeshGitNode();
	virtual 		~MeshGitNode();
	//virtual MStatus compute(const MPlug& plug, MDataBlock& data);
    virtual MStatus      		deform(MDataBlock& 		block,
									   MItGeometry& 	iter,									   
									   unsigned int		multiIndex);
	static  void*	creator();
	static  MStatus initialize();
	static MObject	originalMesh;
	static MObject	derivativeAMesh;
	static MObject	derivativeBMesh;
	static MObject	mergedMesh;
	static MTypeId	id;
	


protected:

};
//END HEADER