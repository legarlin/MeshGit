#include <maya/MPxLocatorNode.h>


class MeshGitFn;
class MeshGitLocatorNode: public MPxLocatorNode 
{

public:

	MeshGitLocatorNode() { }
	virtual ~MeshGitLocatorNode() { }

	//Registration Functions
	static void *creator();
	static MStatus initialize();

	//Drawing function
	virtual void draw(M3dView & view, const MDagPath & path,
					  M3dView::DisplayStyle style,
					  M3dView::DisplayStatus status);

	void drawUnmatched(MeshGitFn &fn);
	void drawMatched(MeshGitFn &fn);

	//Data
	static MTypeId id;

	//Objects
	static MObject meshGitNodeConnection;

	//How much to translate the derivative meshes so we can see them
	//Must match up with setAttr lines in mel file 
	static const int dATranslateX = 0;
	static const int dATranslateY = 2;
	static const int dATranslateZ = 1;
	static const int dBTranslateX = 0;
	static const int dBTranslateY = 2;
	static const int dBTranslateZ = -1;

	//test functions
	static void MeshGitLocatorNode::reportError(MStatus status );
};