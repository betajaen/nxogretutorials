/** 
    
    NxOgre Tutorials - 702 - Suspension and Torque
    
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

class _OgreSampleClassExport NxOgre702 : public SdkSample
{
 
public:
 
 NxOgre::World*          mWorld;
 NxOgre::Scene*          mScene;
 Critter::RenderSystem*  mRenderSystem;
 
 Body*                   mUnicycle;
 Wheel*                  mFrontWheel;
 Wheel*                  mBackWheel;
 
 // So we can identify with shape is which later.
 enum ShapeIds
 {
  Chassis     = 1,
  FrontWheel  = 2,
  BackWheel   = 3
 };
 
 void setupPhysics()
 {
  
  createWorldSceneEtc();
 
  Critter::BodyDescription bodyDescription;
  bodyDescription.mMass = 80.0f;
  
  // The body is going to have only one wheel (A unicycle) to prevent it falling over, we'll cheat physics and
  // freeze the rotations of this body only.
  bodyDescription.mDynamicRigidbodyFlags += DynamicRigidbodyFlags::FreezeRotationX;
  bodyDescription.mDynamicRigidbodyFlags += DynamicRigidbodyFlags::FreezeRotationZ;
  
  // A body with a wheel can't sleep. So we'll make the sleep counter to ~3 years!
  bodyDescription.mWakeUpCounter = 1E8;
  
  NxOgre::ShapeDescriptions shapes;
  
  // A wheel can't just be attached to a Actor/Body by itself, it needs a chassis for collisions.
  BoxDescription box_desc;
  box_desc.mSize.set(0.5,0.5,1.5);
  box_desc.mId = Chassis;
  
  // Create two wheels this time, one at the front (the steering) and one at the back (the driving)
  WheelDescription front_wheel_desc;
  front_wheel_desc.mRadius = 0.5f;
  front_wheel_desc.mLocalPose = Vec3(0,0,0.5);
  front_wheel_desc.mId = FrontWheel;

  WheelDescription back_wheel_desc;
  back_wheel_desc.mRadius = 0.5f;
  back_wheel_desc.mLocalPose = Vec3(0,0,-0.5);
  back_wheel_desc.mId = BackWheel;
  
  shapes.push_back(&box_desc);
  shapes.push_back(&front_wheel_desc);
  shapes.push_back(&back_wheel_desc);
  
  mUnicycle = mRenderSystem->createBody(shapes, Vec3(0,3,0), "cube.1m.mesh", bodyDescription);
  mUnicycle->getNode()->setScale(Ogre::Vector3(0.5,0.5,1.5));
  
  // Go through the shapes and find our wheel, via the Id that was just set earlier.
  // Remember we don't delete the shapes, the Actor/Body will for us. We're just borrowing them.
  
  mFrontWheel = static_cast<Wheel*>(   mUnicycle->getShapeById(FrontWheel)   );
  mBackWheel = static_cast<Wheel*>(   mUnicycle->getShapeById(BackWheel)   );
  
  // Create some random clutter.
  ShapeDescriptions clutter;
  for (unsigned int i=0;i < 30;i++)
  {
   clutter.push_back(new BoxDescription(Vec3(10,1,1),0,Vec3(0,-0.25,i * 3.0f)));
   mRenderSystem->createSceneNodeEntityPair("cube.1m.mesh", Vec3(0,-0.25,i * 3.0f))->scale(10, 1, 1);
  }
  mScene->createSceneGeometry(clutter);
  for (vector_iterator<ShapeDescription*> desc = clutter.elements(); desc != desc.end(); desc++)
   delete (*desc);
  clutter.remove_all();
  
  // Wheels's aren't normally rendered, but later tutorials will cover this with the Machine class
  // For now, turn on the VisualDebugger so you can see it.
//  mRenderSystem->setVisualisationMode(NxOgre::Enums::VisualDebugger_ShowAll);
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

 Body* makeBox(const Matrix44& globalPose, const Vec3& initialVelocity = Vec3::ZERO, GroupIdentifier group = 0)
 {
  
  Critter::BodyDescription bodyDescription;
  bodyDescription.mMass = 40.0f;
  bodyDescription.mLinearVelocity = initialVelocity;

  BoxDescription box_desc;
  box_desc.mSize.set(1,1,1);
  box_desc.mGroup = group;
  
  Body* box = mRenderSystem->createBody(box_desc, globalPose, "cube.1m.mesh", bodyDescription);
  
  return box;
 }

 bool keyPressed(const OIS::KeyEvent& evt)
 {
  
  // Listen to the IJKL keys to rotate and turn the wheel.
  
  if (evt.key == OIS::KC_I)
   mBackWheel->setMotorTorque(100);   // Forward
  else if (evt.key == OIS::KC_K)
   mBackWheel->setMotorTorque(-100);  // Reverse
  else
   mBackWheel->setMotorTorque(0);     // Apply no torque if forward/reverse isn't down.
  
  if (evt.key == OIS::KC_J)
   mFrontWheel->setSteeringAngle(mFrontWheel->getSteeringAngle() + NxOgre::Radian(0.1)); // Turn left
  else if (evt.key == OIS::KC_L)
   mFrontWheel->setSteeringAngle(mFrontWheel->getSteeringAngle() - NxOgre::Radian(0.1)); // Turn right.
  
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
  
  mCamera->setPosition(10,10,10);
  mCamera->lookAt(0,0.5,0);
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

 NxOgre702()
 {
  mInfo["Title"] = "NxOgre 702";
  mInfo["Description"] = "NxOgre 702 - Suspension and Torque";
  mInfo["Thumbnail"] = "thumb_skybox.png";
  mInfo["Category"] = "Physics";
  mInfo["Help"] = "Use IJKL to drive the bike around";
 }

};
