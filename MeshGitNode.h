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
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MFnMeshData.h>
#include <maya/MIOStream.h>

//HEADER
class MeshGitNode : public MPxNode
{
public:
					MeshGitNode();
	virtual 		~MeshGitNode();
	virtual MStatus compute(const MPlug& plug, MDataBlock& data);
	static  void*	creator();
	static  MStatus initialize();
	static MObject	defaultAngle;
	static MObject	defaultStepSize;
	static MObject	grammarFile;
	static MObject	time;
	static MObject	outputMesh;
	static MTypeId	id;
	


protected:

};
//END HEADER