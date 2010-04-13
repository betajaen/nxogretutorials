#include "102.h"
#include "NxOgre.h"
#include "critter.h"

using namespace NxOgre;
using namespace Critter;

void NxOgre102::setupTutorial()
{
 
 NxOgre::MeshManager* meshMan = NxOgre::MeshManager::getSingleton();
 
 NxOgre::Mesh* barrel = meshMan->load("file://../../media/NxOgre/barrel.nxs", "barrel");

 Critter::BodyDescription bodyDescription;
 bodyDescription.mMass = 40.0f;
 
 for (unsigned int i=0;i < 10;i++)
  mRenderSystem->createBody(NxOgre::ConvexDescription(barrel), Vec3(0,45+(i*2),0), "barrel.mesh", bodyDescription);
 
 NxOgre::Mesh* island = meshMan->load("file://../../media/NxOgre/island.nxs", "island");
 
 mScene->createSceneGeometry(TriangleGeometryDescription(island), Vec3(0, 0, 0));
 
 SceneNode* islandNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(Vector3(0,0,0));
 islandNode->attachObject(mSceneMgr->createEntity("island", "island.mesh"));
 islandNode->setScale(0.3,0.3,0.3);
}

bool NxOgre102::keyPressed(const OIS::KeyEvent& evt)
{
 
 return SdkSample::keyPressed(evt);
}