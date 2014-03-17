#include <maya/MPxLocatorNode.h>

class MeshGitLocatorNode:
		public MPxLocatorNode
{

public:

	MeshGitLocatorNode() {
	   
	}
	virtual ~MeshGitLocatorNode() {
	}

	//Registration Functions
	static void *creator();
	static MStatus initialize();

	//Drawing function
	virtual void draw(M3dView & view, const MDagPath & path,
					  M3dView::DisplayStyle style,
					  M3dView::DisplayStatus status);

	//Data
	static MTypeId id;

	//Objects
	static MObject meshGitNodeConnection;


	//test functions
	static void MeshGitLocatorNode::reportError(MStatus status );
private:



};