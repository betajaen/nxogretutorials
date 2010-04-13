#include "101.h"
#include "NxOgre.h"
#include "critter.h"

using namespace NxOgre;
using namespace Critter;

Body* body;
float maxStrength = 1e7;

void NxOgre101::setupTutorial()
{
 
 Critter::BodyDescription bodyDescription;
 bodyDescription.mMass = 40.0f;
 
 body = mRenderSystem->createBody(NxOgre::BoxDescription(1,1,1), Vec3(0,5,0), "cube.1m.mesh", bodyDescription);
 
}

bool NxOgre101::keyPressed(const OIS::KeyEvent& evt)
{
 
 Vec3 force;
 
 if (evt.key == OIS::KC_I)
  force.z += 1;
 else if (evt.key == OIS::KC_K)
  force.z -= 1;
 else if (evt.key == OIS::KC_J)
  force.x -= 1;
 else if (evt.key == OIS::KC_L)
  force.x += 1;
 else if (evt.key == OIS::KC_U)
  force.y += 1;
 else if (evt.key == OIS::KC_M)
  force.y -= 1;
 
 if (!force.isZero())
  body->addForce(maxStrength * force * mLastTimeStep);
 
 if (evt.key == OIS::KC_T)
  body->setGlobalPosition(Vec3(0,1,0));
 
 return SdkSample::keyPressed(evt);
}