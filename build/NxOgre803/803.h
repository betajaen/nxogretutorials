#ifndef __803_H__
#define __803_H__

#include "OGRE/SdkSample.h"
#include "OGRE/SamplePlugin.h"
#include "NxOgre.h"
#include "critter.h"

using namespace Ogre;
using namespace OgreBites;

class _OgreSampleClassExport NxOgre803 : public SdkSample
{
 
public:
 
 NxOgre::World*            mWorld;
 NxOgre::PrincipalScene*   mPriScene;
 NxOgre::AuxiliaryScene*   mAuxScene;

 NxOgre::TimeController*   mTimeController;
 NxOgre::Material*         mDefaultMaterial;
 NxOgre::Actor*            mCameraActor;
 Critter::RenderSystem*    mPriRenderSystem;
 Critter::RenderSystem*    mAuxRenderSystem;
 float                     mLastTimeStep;
 
 void  setupTutorial();
 
 void setupPhysics();
 
 void stopPhysics();
 
 void setupContent()
 {
  
  mCameraMan->setTopSpeed(15.0f);
  
  ColourValue background = ColourValue(16.f/255.f, 16.f/255.f, 16.f/255.f);
  mViewport->setBackgroundColour(background);
  mSceneMgr->setFog(Ogre::FOG_EXP, background, 0.001, 800, 1000);
  
  // set shadow properties
  mSceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_MODULATIVE);
  mSceneMgr->setShadowColour(ColourValue(0.5, 0.5, 0.5));
  mSceneMgr->setShadowTextureSize(8034);
  mSceneMgr->setShadowTextureCount(1);

  // create a floor mesh resource
  MeshManager::getSingleton().createPlane("floor", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
  Plane(Vector3::UNIT_Y, 0), 1000, 1000, 1,1 , true, 1, 1, 1, Vector3::UNIT_Z);

  // create a floor entity, give it a material, and place it at the origin
  Entity* floor = mSceneMgr->createEntity("Floor", "floor");
  floor->setMaterialName("ground-from-nxogre.org");
  floor->setCastShadows(false);
  mSceneMgr->getRootSceneNode()->attachObject(floor);

  // use a small amount of ambient lighting
  mSceneMgr->setAmbientLight(ColourValue(0.3, 0.3, 0.3));

  // add a bright light above the scene
  Light* light = mSceneMgr->createLight();
  light->setType(Light::LT_POINT);
  light->setPosition(-10, 40, 20);
  light->setSpecularColour(ColourValue::White);
  
  mCamera->setPosition(10,10,10);
  mCamera->lookAt(0,0,0);
  mCamera->setNearClipDistance(0.02f);
  mCamera->setFarClipDistance(1000.0f);
  setupPhysics();
 }
 
 void cleanupContent()
 {
  stopPhysics();
 }
 
 bool frameRenderingQueued(const FrameEvent& evt)
 {
  // Advance NxOgre.
  NxOgre::TimeController::getSingleton()->advance(evt.timeSinceLastFrame);
  
  mLastTimeStep = evt.timeSinceLastFrame;

  // Don't let the camera go underground.
  if (mCamera->getPosition().y < 0.5f)
  {
   Ogre::Vector3 pos = mCamera->getPosition();
   pos.y = 0.5f;
   mCamera->setPosition(pos);
  }

  return SdkSample::frameRenderingQueued(evt);
 }

 bool keyPressed(const OIS::KeyEvent& evt);

 NxOgre803()
 {
  mInfo["Title"] = "NxOgre 803";
  mInfo["Description"] = "NxOgre 803 - Meshes and Resources";
  mInfo["Thumbnail"] = "thumb_nxogre803.png";
  mInfo["Category"] = "Physics";
  mInfo["Help"] = "Lots of meshes";
 }

};



#ifndef OGRE_STATIC_LIB

SamplePlugin* sp;
Sample* s;

extern "C" _OgreSampleExport void dllStartPlugin()
{
	s = new NxOgre803;
	sp = OGRE_NEW OgreBites::SamplePlugin(s->getInfo()["Title"] + " Sample");
	sp->addSample(s);
	Ogre::Root::getSingleton().installPlugin(sp);
}

extern "C" _OgreSampleExport void dllStopPlugin()
{
	Ogre::Root::getSingleton().uninstallPlugin(sp); 
	OGRE_DELETE sp;
	delete s;
}

#endif


#endif
