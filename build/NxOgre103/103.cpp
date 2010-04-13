#include "103.h"
#include "NxOgre.h"
#include "critter.h"

using namespace NxOgre;
using namespace Critter;

NxOgre::Mesh* barrelMesh = 0;

Body* NxOgre103::makeContainer(const Matrix44& globalPose)
{
 
 ShapeDescriptions shapes;
 
 MaterialIdentifier material = mDefaultMaterial->getIdentifier();
 
 BoxDescription shape0(Vec3(3.784, 1.75, 0.1), material, Vec3(0, 0, -0.975));
 BoxDescription shape1(Vec3(3.784, 1.75, 0.1), material, Vec3(0, 0,  0.975));
 BoxDescription shape2(Vec3(3.784, 0.1, 2), material, Vec3(0, -0.85, 0));
 BoxDescription shape3(Vec3(3.784, 0.1, 2), material, Vec3(0, 0.85, 0));
 
 shapes.push_back(&shape0);
 shapes.push_back(&shape1);
 shapes.push_back(&shape2);
 shapes.push_back(&shape3);
 
 BodyDescription description;
 description.mMass = 1000;
  
 Body* body = mRenderSystem->createBody(shapes, globalPose, "container.mesh", description);
 
 return body;
}

Body* NxOgre103::makeBarrel(const Matrix44& globalPose, const Vec3& initialVelocity)
{
 
 if (barrelMesh == 0)
  barrelMesh = NxOgre::MeshManager::getSingleton()->load("file://../../media/NxOgre/barrel.nxs", "barrel");
 
 Critter::BodyDescription bodyDescription;
 bodyDescription.mMass = 40.0f;
 bodyDescription.mLinearVelocity = initialVelocity;
 
 Body* body = mRenderSystem->createBody(NxOgre::ConvexDescription(barrelMesh), globalPose, "barrel.mesh", bodyDescription);
 
 return body;
}

void NxOgre103::setupTutorial()
{
 makeContainer(Vec3(3.5,5, 0));
 makeContainer(Vec3(0,3, 0));
 
}

bool NxOgre103::keyPressed(const OIS::KeyEvent& evt)
{
 if (evt.key == OIS::KC_1)
  makeBarrel(Matrix44(Vec3(2,4,0), Quat(0.707, Vec3(1,0,0))), Vec3(1,0,0));
 
 return SdkSample::keyPressed(evt);
}