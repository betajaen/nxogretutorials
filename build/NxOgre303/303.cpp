#include "303.h"
#include "NxOgre.h"
#include "critter.h"

using namespace NxOgre;
using namespace Critter;

class callback : public NxOgre::Callback
{
 public:
  
  callback() : NxOgre::Callback() {}
  
 ~callback() {}
  
  void onContact(const ContactPair& pair)
  {
   if (pair.mFirst->getRigidBodyType() != Critter::Enums::RigidBodyType_Body)
    return;
   Body* body = static_cast<Body*>(pair.mFirst);
   body->setLinearVelocity(body->getLinearVelocity() + Vec3(0,1.0,0));
  }
  
};

callback* cb;

void NxOgre303::setupTutorial()
{
 cb = new callback();
 
 Critter::BodyDescription desc;
 desc.mContactReportFlags |= NxOgre::Enums::ContactPairFlags_All;
 
 Body* a = mRenderSystem->createBody(BoxDescription(Vec3(1,1,1)), Vec3(0.5,5,0.5), "cube.1m.mesh", desc);
 a->setContactCallback(cb);
 Body* b = mRenderSystem->createBody(BoxDescription(Vec3(1,1,1)), Vec3(0, 1, 0), "cube.1m.mesh", desc);
 b->setContactCallback(cb);

}

bool NxOgre303::keyPressed(const OIS::KeyEvent& evt)
{
 return SdkSample::keyPressed(evt);
}