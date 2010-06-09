/** 
    
    NxOgre Tutorials - Sandbox
    
    Copyright (c) 2009 Robin Southern, http://www.nxogre.org
    
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:
    
    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.
    
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
    
*/

#include "OGRE/SdkSample.h"
#include "OGRE/SamplePlugin.h"
#include "NxOgre.h"
#include "critter.h"

using namespace Ogre;
using namespace OgreBites;
using namespace NxOgre;
using namespace Critter;


class _OgreSampleClassExport Sandbox : public SdkSample
{
 
public:
 
 NxOgre::World*          mWorld;
 NxOgre::Scene*          mScene;
 Critter::RenderSystem*  mRenderSystem;

 void setupPhysics()
 {
  mWorld = NxOgre::World::createWorld();
  mWorld->getRemoteDebugger()->connect();

  NxOgre::SceneDescription scene_description;
  scene_description.mGravity = NxOgre::Constants::MEAN_EARTH_GRAVITY;
  
  mScene = mWorld->createScene(scene_description);
  mScene->getMaterial(0)->setAll(0.1,0.9,0.5);
  mRenderSystem = new Critter::RenderSystem(mScene, mSceneMgr);
  
  // USER SETUP CODE HERE.
 }

 void stopPhysics()
 {
  NxOgre::World::destroyWorld();
 }
 
 bool keyPressed(const OIS::KeyEvent& evt)
 {
  return SdkSample::keyPressed(evt);
 }

 void setupContent()
 {
  
  ColourValue background = ColourValue(16.f/255.f, 16.f/255.f, 16.f/255.f);
  mViewport->setBackgroundColour(background);
  mSceneMgr->setFog(Ogre::FOG_EXP, background, 0.001, 800, 1000);
  
  mSceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_MODULATIVE);
  mSceneMgr->setShadowColour(ColourValue(0.5, 0.5, 0.5));
  mSceneMgr->setShadowTextureSize(1024);
  mSceneMgr->setShadowTextureCount(1);

  Ogre::MeshManager::getSingleton().createPlane("floor", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
  Plane(Vector3::UNIT_Y, 0), 1000, 1000, 1,1 , true, 1, 1, 1, Vector3::UNIT_Z);

  Entity* floor = mSceneMgr->createEntity("Floor", "floor");
  floor->setMaterialName("ground-from-nxogre.org");
  floor->setCastShadows(false);
  mSceneMgr->getRootSceneNode()->attachObject(floor);

  mSceneMgr->setAmbientLight(ColourValue(0.3, 0.3, 0.3));

  Light* light = mSceneMgr->createLight();
  light->setType(Light::LT_POINT);
  light->setPosition(-10, 40, 20);
  light->setSpecularColour(ColourValue::White);
  
  mCamera->setPosition(10,10,10);
  mCamera->lookAt(0,0,0);
  mCamera->setNearClipDistance(0.02f);
  mCamera->setFarClipDistance(1000.0f);
  mCameraMan->setTopSpeed(7.5);
  setDragLook(true);
  setupPhysics();
 }
 
 void cleanupContent()
 {
  stopPhysics();
 }
 
 bool frameRenderingQueued(const FrameEvent& evt)
 {
  mWorld->advance(evt.timeSinceLastFrame);
  return SdkSample::frameRenderingQueued(evt);
 }

 Sandbox()
 {
  mInfo["Title"] = "Sandbox";
  mInfo["Description"] = "Sandbox for NxOgre";
  mInfo["Thumbnail"] = "thumb_skybox.png";
  mInfo["Category"] = "Physics";
  mInfo["Help"] = "Go play";
 }

};

SamplePlugin* sp;
Sample* s;

extern "C" _OgreSampleExport void dllStartPlugin()
{
	s = new Sandbox;
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
