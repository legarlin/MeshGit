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
MObject MeshGitNode::defaultAngle;
MObject MeshGitNode::defaultStepSize;
MObject MeshGitNode::grammarFile;//TO DO GRAMMAR
MObject MeshGitNode::time;
MObject MeshGitNode::outputMesh;
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
	MFnUnitAttribute unitAttr;
	MFnTypedAttribute typedAttr;
	MFnNumericAttribute numericAttr;
	MFnStringData stringFn;

	MStatus returnStatus;

	//Create the attributes
	MObject holderS=  stringFn.create("Type in Grammar");
	MeshGitNode::grammarFile = typedAttr.create("grammarText","gt",MFnData::kString, holderS, &returnStatus);
	McheckErr(returnStatus, "ERROR creating MeshGitNode string grammarFile\n");
	returnStatus = addAttribute(MeshGitNode::grammarFile);
	McheckErr(returnStatus, "ERROR adding grammarFile\n");


	MeshGitNode::time = unitAttr.create( "time", "tm",
										  MFnUnitAttribute::kTime,
										  0.0, &returnStatus );
	McheckErr(returnStatus, "ERROR creating MeshGitNode time attribute\n");
	returnStatus = addAttribute(MeshGitNode::time);
	McheckErr(returnStatus, "ERROR adding time attribute\n");

	MeshGitNode::defaultAngle = numericAttr.create( "defaultAngle", "da",
										  MFnNumericData::kFloat ,
										  25.7, &returnStatus );
	McheckErr(returnStatus, "ERROR creating MeshGitNode time attribute\n");
	numericAttr.setSoftMax(360);
	returnStatus = addAttribute(MeshGitNode::defaultAngle);
	McheckErr(returnStatus, "ERROR adding defaultAngle attribute\n");

	MeshGitNode::defaultStepSize = numericAttr.create( "defaultStep", "ds",
										  MFnNumericData::kFloat ,
										  0.5, &returnStatus );
	McheckErr(returnStatus, "ERROR creating MeshGitNode time attribute\n");
	numericAttr.setSoftMax(3);
	returnStatus = addAttribute(MeshGitNode::defaultStepSize);
	McheckErr(returnStatus, "ERROR adding defaultStepSize attribute\n");

	MeshGitNode::outputMesh = typedAttr.create( "outputMesh", "out",
												 MFnData::kMesh,
												 &returnStatus ); 
	McheckErr(returnStatus, "ERROR creating MeshGitNode output attribute\n");
	typedAttr.setStorable(false);
	returnStatus = addAttribute(MeshGitNode::outputMesh);
	McheckErr(returnStatus, "ERROR adding outputMesh attribute\n");

	//Set the attribute affects
	returnStatus = attributeAffects(MeshGitNode::time, MeshGitNode::outputMesh);
	McheckErr(returnStatus, "AttributeAffects error for 'blendEndFrames'");
	returnStatus = attributeAffects(MeshGitNode::defaultAngle, MeshGitNode::outputMesh);
	McheckErr(returnStatus, "AttributeAffects error for 'blendEndFrames'");
	returnStatus = attributeAffects(MeshGitNode::defaultStepSize, MeshGitNode::outputMesh);
	McheckErr(returnStatus, "AttributeAffects error for 'blendEndFrames'");
	returnStatus = attributeAffects(MeshGitNode::grammarFile, MeshGitNode::outputMesh);
	McheckErr(returnStatus, "AttributeAffects error for 'blendEndFrames'");

	std::cout << "RELOADED" << std::endl;

	return MS::kSuccess;
}


MStatus MeshGitNode::compute(const MPlug& plug, MDataBlock& data)

{
	MStatus returnStatus;
	MGlobal::displayInfo("Compute Called!");
	MString command = "print( \"compute called \");";
	MGlobal::executeCommand(command);
	
	if (plug == outputMesh) {

		///* Get time */
		MDataHandle timeData = data.inputValue( time, &returnStatus ); 
		McheckErr(returnStatus, "Error getting time data handle\n");
		int time = timeData.asTime().value();

		MDataHandle angleData = data.inputValue( MeshGitNode::defaultAngle, &returnStatus ); 
		McheckErr(returnStatus, "Error getting time angle handle\n");
		float angle = angleData.asFloat();

		MDataHandle stepData = data.inputValue( MeshGitNode::defaultStepSize, &returnStatus ); 
		McheckErr(returnStatus, "Error getting time angle handle\n");
		float step = stepData.asFloat();

		MDataHandle grammarData = data.inputValue( MeshGitNode::grammarFile, &returnStatus ); 
		McheckErr(returnStatus, "Error getting time angle handle\n");
		MString grammarFile = grammarData.asString();


		///* Get output object */
		MDataHandle outputHandle = data.outputValue(outputMesh, &returnStatus);
		McheckErr(returnStatus, "ERROR getting polygon data handle\n");

		//MFnMeshData dataCreator;
		//MObject newOutputData = dataCreator.create(&returnStatus);
		//McheckErr(returnStatus, "ERROR creating outputData");
		MFnMeshData dataCreator;
		MObject newOutputData = dataCreator.create(&returnStatus);
		McheckErr(returnStatus, "ERROR creating outputData");


		outputHandle.set(newOutputData);    
		data.setClean( plug );
	} else
		return MS::kUnknownParameter;

	return MS::kSuccess;
}


