/** 
    
    NxOgre Tutorials - 107 - Rigidbody Properties
    
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

class _OgreSampleClassExport NxOgre107 : public SdkSample
{
 
public:
 
 NxOgre::World*          mWorld;
 NxOgre::Scene*          mScene;

 
 float                   mLastTimeStep;
 NxOgre::Material*       mDefaultMaterial;
 Critter::RenderSystem*  mRenderSystem;
 Critter::Body*          mSelectedBody;
 OgreBites::Button*      mAddNew;
 OgreBites::Button*      mResetSelected;
 OgreBites::Button*      mResetAll;
 OgreBites::SelectMenu*  mSelectedBodies;
 OgreBites::Slider*      mSelectedMass;
 OgreBites::Slider*      mLinearVelocityX;
 OgreBites::Slider*      mLinearVelocityY;
 OgreBites::Slider*      mLinearVelocityZ;
 OgreBites::Slider*      mAngularVelocityX;
 OgreBites::Slider*      mAngularVelocityY;
 OgreBites::Slider*      mAngularVelocityZ;
 NxOgre::map<std::string, Critter::Body*>  mBodies;
 unsigned int            mNextBodyID;
 
 void setupPhysics()
 {
  
  // Create the world.
  mWorld = NxOgre::World::createWorld();
  
  // Allow NxOgre to use the Ogre resource system, via the Critter OgreResourceProtocol class.
  // - ResourceProtocols are normally owned by the ResourceSystem, so we don't have to delete it later.
  NxOgre::ResourceSystem::getSingleton()->openProtocol(new Critter::OgreResourceProtocol());

  mWorld->getRemoteDebugger()->connect();

  // Create the time controller
  

  // Create the scene
  NxOgre::SceneDescription scene_description;
  scene_description.mGravity = NxOgre::Constants::MEAN_EARTH_GRAVITY;
  scene_description.mUseHardware = false;

  mScene = mWorld->createScene(scene_description);
  
  // Set default material properties
  mDefaultMaterial = mScene->getMaterial(0);
  mDefaultMaterial->setRestitution(0.1f);
  mDefaultMaterial->setDynamicFriction(0.9);
  mDefaultMaterial->setStaticFriction(0.5);
  
  // Plane creation
  mScene->createSceneGeometry(NxOgre::PlaneGeometryDescription());

  // Create the rendersystem.
  mRenderSystem = new Critter::RenderSystem(mScene, mSceneMgr);
  
  // mRenderSystem->setVisualisationMode(NxOgre::Enums::VisualDebugger_ShowAll);
  
  // Setup a BodyDescription.
  Critter::BodyDescription bodyDescription;
  bodyDescription.mMass = 40.0f; // Set the mass to 40kg.
  
  mNextBodyID = 0;
  
  makeBox(Vec3(-2,5,0), Vec3(0,0,1));
  
 }

 void stopPhysics()
 {
  NxOgre::World::destroyWorld();
 }
 
 Body* makeBox(const Matrix44& globalPose, const Vec3& initialVelocity)
 {
  
  Critter::BodyDescription bodyDescription;
  bodyDescription.mMass = 40.0f;
  bodyDescription.mLinearVelocity = initialVelocity;
  bodyDescription.mName = "Body" + to_s(mNextBodyID++);
  
  mSelectedBody = mRenderSystem->createBody(BoxDescription(1,1,1), globalPose, "cube.1m.mesh", bodyDescription);
  mBodies.insert(bodyDescription.mName, mSelectedBody);
  mSelectedBodies->addItem(bodyDescription.mName);
  mSelectedBodies->selectItem(bodyDescription.mName, true);
  
  return mSelectedBody;
 }

 bool keyPressed(const OIS::KeyEvent& evt)
 {
  
  if (evt.key == OIS::KC_1)
   makeBox(Matrix44(Vec3(-4,5,0), Quat(0.707, Vec3(1,0,0))), Vec3(1,1,0));
  
  return SdkSample::keyPressed(evt);
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

  
  mTrayMgr->showCursor();
  
  mAddNew = mTrayMgr->createButton(OgreBites::TL_TOPLEFT, "AddNew", "Add New");
  mResetAll = mTrayMgr->createButton(OgreBites::TL_TOPLEFT, "ResetAll", "Reset All");

  mTrayMgr->createLabel(OgreBites::TL_RIGHT, "SelectedCaption", "Selected");
  mSelectedBodies = mTrayMgr->createLongSelectMenu(OgreBites::TL_RIGHT, "SelectedName", "Name", 300, 200, 5);
  mSelectedMass = mTrayMgr->createLongSlider(OgreBites::TL_RIGHT, "SelectedMass", "Mass", 300, 100, 100, 0.001, 2000, 100);
  
  mResetSelected = mTrayMgr->createButton(OgreBites::TL_RIGHT, "SelectedReset", "Reset", 200);
  
  mTrayMgr->createLabel(OgreBites::TL_RIGHT, "SelectedLinVelCaption", "Linear Velocity");
  mLinearVelocityX = mTrayMgr->createLongSlider(OgreBites::TL_RIGHT, "SelectedLinVelx", "X", 300, 100, 100, -1000, 1000, 100);
  mLinearVelocityY = mTrayMgr->createLongSlider(OgreBites::TL_RIGHT, "SelectedLinVely", "Y", 300, 100, 100, -1000, 1000, 100);
  mLinearVelocityZ = mTrayMgr->createLongSlider(OgreBites::TL_RIGHT, "SelectedLinVelz", "Z", 300, 100, 100, -1000, 1000, 100);
  
  mTrayMgr->createLabel(OgreBites::TL_RIGHT, "SelectedAngVelCaption", "Angular Velocity");
  mAngularVelocityX = mTrayMgr->createLongSlider(OgreBites::TL_RIGHT, "SelectedAngVelx", "X", 300, 100, 100, -1000, 1000, 100);
  mAngularVelocityY = mTrayMgr->createLongSlider(OgreBites::TL_RIGHT, "SelectedAngVely", "Y", 300, 100, 100, -1000, 1000, 100);
  mAngularVelocityZ = mTrayMgr->createLongSlider(OgreBites::TL_RIGHT, "SelectedAngVelz", "Z", 300, 100, 100, -1000, 1000, 100);
  
  setDragLook(true);
  mCameraMan->setTopSpeed(7.5);
  
  
  setupPhysics();
 }
 
 void cleanupContent()
 {
  stopPhysics();
 }
 
 bool frameRenderingQueued(const FrameEvent& evt)
 {
  
  mWorld->advance(evt.timeSinceLastFrame);
  mLastTimeStep = mScene->getTimeStep().getModified();
  
  if (mSelectedBody)
  {
   
   mSelectedMass->setValue(mSelectedBody->getMass());
   
   Vec3 linVel = mSelectedBody->getLinearVelocity();
   mLinearVelocityX->setValue(linVel.x);
   mLinearVelocityY->setValue(linVel.y);
   mLinearVelocityZ->setValue(linVel.z);
   
   Vec3 angVel = mSelectedBody->getAngularVelocity();
   mAngularVelocityX->setValue(angVel.x);
   mAngularVelocityY->setValue(angVel.y);
   mAngularVelocityZ->setValue(angVel.z);
   
  }
  
  // Don't let the camera go underground.
  if (mCamera->getPosition().y < 0.5f)
  {
   Ogre::Vector3 pos = mCamera->getPosition();
   pos.y = 0.5f;
   mCamera->setPosition(pos);
  }

  return SdkSample::frameRenderingQueued(evt);
 }

 void sliderMoved(Slider* slider)
 {

  if (mSelectedBody && slider == mSelectedMass)
  {
   mSelectedBody->setMass(slider->getValue());
  }

  if (mSelectedBody && slider == mLinearVelocityX)
  {
   Vec3 linVel = mSelectedBody->getLinearVelocity();
   linVel.x = slider->getValue();
   mSelectedBody->setLinearVelocity(linVel);
  }

  if (mSelectedBody && slider == mLinearVelocityY)
  {
   Vec3 linVel = mSelectedBody->getLinearVelocity();
   linVel.y = slider->getValue();
   mSelectedBody->setLinearVelocity(linVel);
  }

  if (mSelectedBody && slider == mLinearVelocityZ)
  {
   Vec3 linVel = mSelectedBody->getLinearVelocity();
   linVel.z = slider->getValue();
   mSelectedBody->setLinearVelocity(linVel);
  }

  if (mSelectedBody && slider == mAngularVelocityX)
  {
   Vec3 angVel = mSelectedBody->getAngularVelocity();
   angVel.x = slider->getValue();
   mSelectedBody->setAngularVelocity(angVel);
  }

  if (mSelectedBody && slider == mAngularVelocityY)
  {
   Vec3 angVel = mSelectedBody->getAngularVelocity();
   angVel.y = slider->getValue();
   mSelectedBody->setAngularVelocity(angVel);
  }

  if (mSelectedBody && slider == mAngularVelocityZ)
  {
   Vec3 angVel = mSelectedBody->getAngularVelocity();
   angVel.z = slider->getValue();
   mSelectedBody->setAngularVelocity(angVel);
  }

 }

 void itemSelected(SelectMenu* menu)
 {
  if (menu != mSelectedBodies)
   return;
  
  Ogre::String name = menu->getSelectedItem();
  if (!mBodies.has(name))
   return;
  
  mSelectedBody = mBodies.at(name);
  
 }

 void buttonHit(Button* button)
 {
  
  if (mSelectedBody && mResetSelected == button)
  {
   mSelectedBody->setMass(40.0f);
   mSelectedBody->setLinearVelocity(Vec3::ZERO);
   mSelectedBody->setAngularVelocity(Vec3::ZERO);
   mSelectedBody->setGlobalPosition(Vec3(0,0.5,0));
   mSelectedBody->setGlobalOrientation(Quat::IDENTITY);
  }
  
  if (mResetAll == button)
  {
   float y = 0.5;
   for (NxOgre::map_iterator<std::string, Critter::Body*> it = mBodies.elements(); it != it.end(); it++)
   {
    it->setMass(40.0f);
    it->setLinearVelocity(Vec3::ZERO);
    it->setAngularVelocity(Vec3::ZERO);
    it->setGlobalPosition(Vec3(0,y,0));
    it->setGlobalOrientation(Quat::IDENTITY);
    y+=1;
   }
  }
  
  if (mAddNew == button)
  {
   makeBox(Matrix44(Vec3(-4,5,0), Quat(0.707, Vec3(1,0,0))), Vec3(1,1,0));
  }
  
 }

 NxOgre107() : mSelectedBody(0)
 {
  mInfo["Title"] = "NxOgre 107";
  mInfo["Description"] = "NxOgre 107 - Rigidbody Properties";
  mInfo["Thumbnail"] = "thumb_skybox.png";
  mInfo["Category"] = "Physics";
  mInfo["Help"] = "Looking at and changing RigidBody Properties";
 }

};

SamplePlugin* sp;
Sample* s;

extern "C" _OgreSampleExport void dllStartPlugin()
{
 s = new NxOgre107;
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
