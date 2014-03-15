#include <maya/MDagPath.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnMessageAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MMatrix.h>
#include <maya/MPlugArray.h>
#include <MeshGitLocatorNode.h>

#define scopeName "MeshGitLocatorNode"
MTypeId MeshGitLocatorNode::id(0x03AA4);

using namespace std;

void * MeshGitLocatorNode::creator() {
    return new MeshGitLocatorNode;
}

MStatus MeshGitLocatorNode::initialize() {
    MStatus status;
    MFnNumericAttribute nAttr;
    MFnMessageAttribute msAttr;
    MFnEnumAttribute eAttr;

    return status;
}

void MeshGitLocatorNode::draw(M3dView & view, const MDagPath & path,
                              M3dView::DisplayStyle ,
                              M3dView::DisplayStatus ) {
    MStatus status;
    MObject thisNode = thisMObject();

    
    //Display dependency node
    MObject thisObj = thisMObject();
    MFnDependencyNode nodeFn(thisObj);
    MMatrix worldToDisplay = path.inclusiveMatrixInverse(&status);

    view.beginGL();
    glPushAttrib(GL_CURRENT_BIT | GL_LINE_BIT);
    glPushMatrix();
    float matrix[4][4];
    worldToDisplay.get(matrix);
    glMultMatrixf(&matrix[0][0]);

    glPointSize(1.0f);
    glLineWidth(1.0f);

    glPopMatrix();
    glPopAttrib();
    view.endGL();

}
