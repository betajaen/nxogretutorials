/** 
    
    NxOgre Tutorials - 304a - Volumes with Functions
    
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

class _OgreSampleClassExport NxOgre304a : public SdkSample
{
 
public:
 
 NxOgre::World*          mWorld;
 NxOgre::Scene*          mScene;
 Critter::RenderSystem*  mRenderSystem;
 NxOgre::Vec3                    mTarget;
 void setupPhysics()
 {
  
  createWorldSceneEtc();
  
  
  // Collector tube at the bottom
  createVolume(  Vec3(8,16,8), Vec3(0,8,0), VolumePhysicsFunction::move_to(Vec3(0,18,0), 50)  );
  
  // Long transport tube
  createVolume(  Vec3(100,8,8), Vec3(-50+4, 16+4, 0), VolumePhysicsFunction::move_to(Vec3(-100,18,0), 50)  );

  // Upper transport tube
  createVolume(  Vec3(8,32,8), Vec3(-100,16+16,0), VolumePhysicsFunction::move_to(Vec3(-100,80,0), 50)  );

  // Firing tube
  createVolume(  Vec3(8,4,8), Vec3(-100,16+32+2,0), VolumePhysicsFunction::move_to(Vec3(-150,100,0), 50)  );

  // Catching area.
  createVolume(  Vec3(75,4,75), Vec3(-400,16,0), VolumePhysicsFunction::teleport_to(Vec3(0,0.5,0))  );

  // Create a couple of boxes.
  int x = 0;
  int y = 0;
  for (unsigned int i=0;i < 100;i++)
   makeBox(Vec3(x == 0 ? x++ : x--,0.5f + i,y == 0 ? y++ : y--));

 }
 
 Volume* createVolume(const Vec3& size, const Vec3& position, NxOgre::Callback* callback)
 {
  Volume* volume = mScene->createVolume(BoxDescription(size), position, callback);

  // make it visible, via a scenenode.
  Ogre::SceneNode* node = mRenderSystem->createSceneNodeEntityPair("cube.mesh", volume->getGlobalPosition());
  node->setScale(size.as<Ogre::Vector3>() * 0.01);
  static_cast<Ogre::Entity*>(node->getAttachedObject(0))->setMaterialName("nxogre.forcefield2");

  return volume;
 }



  
 Body* makeBox(const Matrix44& globalPose, const Vec3& initialVelocity = Vec3::ZERO)
 {
  
  Critter::BodyDescription bodyDescription;
  bodyDescription.mMass = 40.0f;
  bodyDescription.mLinearVelocity = initialVelocity;
  
  BoxDescription box_desc;
  box_desc.mSize.set(1,1,1);
  
  Body* box = mRenderSystem->createBody(box_desc, globalPose, "cube.1m.mesh", bodyDescription);
  
  return box;
 }

 void createWorldSceneEtc()
 {
  // The usual suspects.
  mWorld = NxOgre::World::createWorld();
  NxOgre::ResourceSystem::getSingleton()->openProtocol(new Critter::OgreResourceProtocol());
  mWorld->getRemoteDebugger()->connect();
  NxOgre::SceneDescription scene_description;
  scene_description.mGravity = Constants::MEAN_EARTH_GRAVITY;
  scene_description.mUseHardware = false;
  mScene = mWorld->createScene(scene_description);
  mScene->createSceneGeometry(NxOgre::PlaneGeometryDescription());
  mRenderSystem = new Critter::RenderSystem(mScene, mSceneMgr);
  mScene->getMaterial(0)->setAll(0.1, 0.5,0.5);
 }

 bool keyPressed(const OIS::KeyEvent& evt)
 {
  return SdkSample::keyPressed(evt);
 }
 void stopPhysics()
 {
  NxOgre::World::destroyWorld();
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
  
  mCamera->setPosition(20,40,20);
  mCamera->lookAt(0,15,0);
  mCamera->setNearClipDistance(0.02f);
  mCamera->setFarClipDistance(1000.0f);
  mCameraMan->setTopSpeed(7.5);

  setupPhysics();
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

 NxOgre304a()
 {
  mInfo["Title"] = "NxOgre 304a";
  mInfo["Description"] = "NxOgre 304a - Volumes with Functions";
  mInfo["Thumbnail"] = "thumb_skybox.png";
  mInfo["Category"] = "Physics";
  mInfo["Help"] = "Filtering collisions.";
 }

};
