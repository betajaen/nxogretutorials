/** 
    
    NxOgre Tutorials - 1201 - Background Animated Characters
    
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

using namespace NxOgre;
using namespace Critter;
using namespace Ogre;
using namespace OgreBites;


class _OgreSampleClassExport NxOgre1201 : public SdkSample
{
 
public:
 
 OIS::JoyStick*                  mJoyStick;
 NxOgre::World*                  mWorld;
 NxOgre::Scene*                  mScene;
 Critter::RenderSystem*          mRenderSystem;
 Ogre::Real                      mCameraYaw, mCameraPitch;
 Ogre::Vector3                   mCameraOffset;
 Critter::BackgroundCharacter*   mSinbad;
 Critter::CharacterInputHelper   mSinbadHelper;
 NxOgre::Actor*                  mTestActor;

 enum ControllerShapeGroups
 {
  NonCollidable = 0,      // Things that the character controller can go through such as Ghosts or tiny
                          // objects like small rocks.
  Walls = 1,              // Walls, Floors and other static geometry that can't be moved.
  Objects = 2             // Boxes, Barrels and other dynamic parts of the scene that can be moved by pushing.
 };

 enum SinbadSections
 {
  SinbadLower,
  SinbadUpper
 };
 
 void setupPhysics()
 {
  // Create everything Quickly.
  mWorld = NxOgre::World::createWorld();
  NxOgre::SceneDescription scene_desc;
  scene_desc.mGravity = NxOgre::Constants::MEAN_EARTH_GRAVITY;
  mScene = mWorld->createScene(scene_desc);
  mRenderSystem = new Critter::RenderSystem(mScene, mSceneMgr);
  mRenderSystem->createVisualDebugger();
  
  // Fake Floor
  BoxDescription fake_floor_desc(250,1,250);
  fake_floor_desc.mGroup = Walls;
  mScene->createSceneGeometry(fake_floor_desc, Vec3(0,-0.5,0));

  // Setup Animations. 
  mRenderSystem->addAnimation("sinbad.mesh", SinbadLower, Critter::Enums::StockAnimationID_Idle, "IdleBase");
  mRenderSystem->addAnimation("sinbad.mesh", SinbadUpper, Critter::Enums::StockAnimationID_Idle, "IdleTop");
  mRenderSystem->addAnimation("sinbad.mesh", SinbadLower, Critter::Enums::StockAnimationID_Forward, "RunBase");
  mRenderSystem->addAnimation("sinbad.mesh", SinbadUpper, Critter::Enums::StockAnimationID_Forward, "RunTop");
  mRenderSystem->addAnimation("sinbad.mesh", SinbadLower, Critter::Enums::StockAnimationID_Jump, "JumpStart", 5.0, false);
  mRenderSystem->addAnimation("sinbad.mesh", SinbadLower, Critter::Enums::StockAnimationID_Fall, "JumpLoop");
  mRenderSystem->addAnimation("sinbad.mesh", SinbadLower, Critter::Enums::StockAnimationID_Land, "JumpEnd", 5.0, false);
  mRenderSystem->addAnimation("sinbad.mesh", SinbadLower, 100, "Dance", 5.0, false);
  
  Critter::BackgroundCharacterDescription desc;
  desc.mShape = NxOgre::SimpleCapsule(5+0.5+0.1,2);
  desc.mCollisionMask = (Walls << 1) | (Objects << 1);
  desc.mMaxGroundSpeed = 17.0f;
  desc.setJumpVelocityFromMaxHeight(mScene->getGravity().y, 0.75f);
  
  mSinbad = mRenderSystem->createBackgroundCharacter(Ogre::Vector3(0,50,0), Ogre::Radian(0), "sinbad.mesh", desc);
  
  
  NxOgre::RigidBodyDescription test_desc;
  test_desc.mDynamicRigidbodyFlags += NxOgre::DynamicRigidbodyFlags::FreezeRotation;
  
  mTestActor = mScene->createActor(desc.mShape.to_desc(), NxOgre::Vec3(5,50,0), test_desc);
 }
 
 bool frameRenderingQueued(const FrameEvent& evt)
 {
  
  mSinbadHelper.reset();

  if (mKeyboard->isKeyDown(OIS::KC_1))
  {
   mSinbad->setPosition(Vec3(0,10,0));
   mTestActor->setGlobalPosition(Vec3(5,10,0));
   mTestActor->wakeUp(1E5);
  }
  if (mKeyboard->isKeyDown(OIS::KC_2))
  {
   mSinbad->setPosition(Vec3(0,50,0));
   mTestActor->setGlobalPosition(Vec3(5,50,0));
   mTestActor->wakeUp(1E5);
  }
  
  if (mKeyboard->isKeyDown(OIS::KC_3))
  {
   mSinbad->playAnimation(100, Critter::Enums::NO_ANIMATION, Critter::Enums::StockAnimationID_Idle, Critter::Enums::StockAnimationID_Idle, true);
  }
  
  if (mKeyboard->isKeyDown(OIS::KC_I))
   mSinbadHelper.forward(127);
  else if (mKeyboard->isKeyDown(OIS::KC_K))
  {
   mSinbadHelper.backward(127);
  }
  
  if (mKeyboard->isKeyDown(OIS::KC_J))
   mSinbadHelper.left(127);
  else if (mKeyboard->isKeyDown(OIS::KC_L))
  {
   mSinbadHelper.right(127);
  }
  
  if (mKeyboard->isKeyDown(OIS::KC_SPACE))
  {
   mSinbadHelper.up(true);
  }
  
  if (mJoyStick != 0)
  {
   
   mJoyStick->capture();
   
   const OIS::JoyStickState& js = mJoyStick->getJoyStickState();
   
   // This is written with an XBOX 360 Controller in mind, and will probably be different for other kinds.
   // 0 - Up/Down (Forward, backward)
   // 1 - Left/Right (Turn left, Turn Right)
   // Bumper Left (Side step left, Side step right).
   
   float margin = 0.35;

   float forward_backward = float(-js.mAxes[0].abs) / 32768.0f;
   float left_right       = float(-js.mAxes[1].abs) / 32768.0f;
   
   if (js.mAxes[1].abs != 0)
    mSinbadHelper.input.is_turning = true;

   if (NxOgre::Math::abs(forward_backward) >= margin)
   {
    forward_backward *= 127;
    mSinbadHelper.input.forward_backward = int(forward_backward);
   }
    
   if (NxOgre::Math::abs(left_right) >= margin)
   {
    left_right *= 127;
    mSinbadHelper.input.left_right = int(left_right);
   }
    
   if (js.mButtons[0])
   {
    mSinbadHelper.input.up = true;
   }
    
  }
  
  mSinbad->setInput(mSinbadHelper);
  
  
  // Advance NxOgre.
  mWorld->advance(evt.timeSinceLastFrame);
  return SdkSample::frameRenderingQueued(evt);
 }

 void stopPhysics()
 {
  NxOgre::World::destroyWorld();
 }
  
 bool mouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
 {
  if (evt.state.buttonDown(OIS::MB_Right) == false)
   SdkSample::mouseReleased(evt, id);
   
  return true;
 }

 bool keyPressed(const OIS::KeyEvent& evt)
 {

  return SdkSample::keyPressed(evt);
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
  // Create the world.
  mWorld = NxOgre::World::createWorld();
    
  // Create the scene
  NxOgre::SceneDescription scene_description;
  scene_description.mGravity = NxOgre::Constants::MEAN_EARTH_GRAVITY;
  scene_description.mUseHardware = false;
  
  mScene = mWorld->createScene(scene_description);
  
  // Set default material properties
  mScene->getMaterial(0)->setAll(0.1f, 0.9f, 0.5f);
  
  // Plane creation
  mScene->createSceneGeometry(NxOgre::PlaneGeometryDescription());
  
  // Create the rendersystem.
  mRenderSystem = new Critter::RenderSystem(mScene, mSceneMgr);
  
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
  mCameraMan->setStyle(CS_FREELOOK);
  
  
  OIS::InputManager* im = mMouse->getCreator();
  
  if (im->getNumberOfDevices(OIS::OISJoyStick) > 0)
  {
   mJoyStick = static_cast<OIS::JoyStick*>( im->createInputObject(OIS::OISJoyStick, false) );
   
   mLabelJoy0 = mTrayMgr->createLabel(OgreBites::TL_RIGHT, "Joy0", "0", 100);
   mLabelJoy1 = mTrayMgr->createLabel(OgreBites::TL_RIGHT, "Joy1", "0", 100);

  }
  
  mVectorDebugObj = mSceneMgr->createManualObject();
  mVectorDebugObj->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_LIST);
  mVectorDebugObj->position(0,0,0);
  mVectorDebugObj->position(0,1,0);
  mVectorDebugObj->end();

  mVectorDebugNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
  mVectorDebugNode->attachObject(mVectorDebugObj);
  
  mCameraYaw = 0;
  mCameraPitch = 0;
  setupPhysics();
 }
 
 void cleanupContent()
 {
  stopPhysics();
 }
 
 NxOgre1201() : mJoyStick(0)
 {
  mInfo["Title"] = "NxOgre 1201";
  mInfo["Description"] = "NxOgre 1201 - Background Animated Characters";
  mInfo["Thumbnail"] = "thumb_skybox.png";
  mInfo["Category"] = "Physics";
  mInfo["Help"] = "Using simple background animated characters and controlling them.";
 }

 OgreBites::Label* mLabelJoy0;
 OgreBites::Label* mLabelJoy1;
 Ogre::ManualObject* mVectorDebugObj;
 Ogre::SceneNode* mVectorDebugNode;
 
};
