/** 
    
    NxOgre Tutorials - 1004 - Cloth Pipes
    
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

class _OgreSampleClassExport NxOgre1004 : public SdkSample
{
 
public:
 
 NxOgre::World*          mWorld;
 NxOgre::Scene*          mScene;
 float                   mLastTimeStep;
 NxOgre::MeshManager*    mMeshManager;
 NxOgre::Material*       mDefaultMaterial;
 Critter::RenderSystem*  mRenderSystem;
 NxOgre::Mesh*           mBalloonMesh;
 
 void setupPhysics()
 {
  
  // Create the world.
  mWorld = NxOgre::World::createWorld();
  
  // Allow NxOgre to use the Ogre resource system, via the Critter OgreResourceProtocol class.
  // - ResourceProtocols are normally owned by the ResourceSystem, so we don't have to delete it later.
  NxOgre::ResourceSystem::getSingleton()->openProtocol(new Critter::OgreResourceProtocol());

  mWorld->getRemoteDebugger()->connect();
  
  // Create the scene
  NxOgre::SceneDescription scene_description;
  scene_description.mGravity = NxOgre::Constants::MEAN_EARTH_GRAVITY;
  scene_description.mUseHardware = false;

  mScene = mWorld->createScene(scene_description);
  
  // Set default material properties
  mScene->getMaterial(0)->setAll(0.1, 0.9, 0.5);
  
  // Get a copy of it's pointer
  mMeshManager = NxOgre::MeshManager::getSingleton();

  // Plane creation
  mScene->createSceneGeometry(NxOgre::PlaneGeometryDescription());

  // Create the rendersystem.
  mRenderSystem = new Critter::RenderSystem(mScene, mSceneMgr);
  // mRenderSystem->setVisualisationMode(NxOgre::Enums::VisualDebugger_ShowAll);
  
  makeBalloon(Vec3(0,5,0));
 }

 void stopPhysics()
 {
  NxOgre::World::destroyWorld();
 }
 
 bool keyPressed(const OIS::KeyEvent& evt)
 {
  return SdkSample::keyPressed(evt);
 }

 void makeBalloon(const Vec3& position)
 {
  
  if (mBalloonMesh == 0)
   mBalloonMesh = NxOgre::MeshManager::getSingleton()->load("ogre://balloon.xcl");

  NxOgre::ClothDescription desc;
  desc.mMesh = mBalloonMesh;
  desc.mThickness = 0.2;
  desc.mFriction = 0.5;
  desc.mPressure = 1.0;
  desc.mFlags |= NxOgre::Enums::ClothFlags_Pressure;
  desc.mGlobalPose.set(position);
  
  mRenderSystem->createCloth(desc, "nxogre.balloon");
  
 }

 void setupContent()
 {
  
  ColourValue background = ColourValue(16.f/255.f, 16.f/255.f, 16.f/255.f);
  mViewport->setBackgroundColour(background);
  mSceneMgr->setFog(Ogre::FOG_EXP, background, 0.001, 800, 1000);
  
  // set shadow properties
  mSceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_MODULATIVE);
  mSceneMgr->setShadowColour(ColourValue(0.5, 0.5, 0.5));
  mSceneMgr->setShadowTextureSize(1024);
  mSceneMgr->setShadowTextureCount(1);

  // create a floor mesh resource
  Ogre::MeshManager::getSingleton().createPlane("floor", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
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
  mCameraMan->setTopSpeed(7.5);
  
  
  setupPhysics();
  setDragLook(true);
 }
 
 void cleanupContent()
 {
  stopPhysics();
 }
 
 bool frameRenderingQueued(const FrameEvent& evt)
 {
  // Advance NxOgre.
  mWorld->advance(evt.timeSinceLastFrame);

  // Don't let the camera go underground.
  if (mCamera->getPosition().y < 0.5f)
  {
   Ogre::Vector3 pos = mCamera->getPosition();
   pos.y = 0.5f;
   mCamera->setPosition(pos);
  }

  return SdkSample::frameRenderingQueued(evt);
 }

 NxOgre1004() : mBalloonMesh(0)
 {
  mInfo["Title"] = "NxOgre 1004";
  mInfo["Description"] = "NxOgre 1004 - Cloth Pipes";
  mInfo["Thumbnail"] = "thumb_skybox.png";
  mInfo["Category"] = "Physics";
  mInfo["Help"] = "Introduction to Cloth Pipes.";
 }

};

SamplePlugin* sp;
Sample* s;

extern "C" _OgreSampleExport void dllStartPlugin()
{
	s = new NxOgre1004;
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
