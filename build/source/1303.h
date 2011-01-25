/** 
    
    NxOgre Tutorials - 1303 - Animation
    
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
#include "Critter.h"

using namespace Ogre;
using namespace OgreBites;

class _OgreSampleClassExport NxOgre1303 : public SdkSample
{
 
public:
 
 NxOgre::World*          mWorld;
 NxOgre::Scene*          mScene;
 Critter::RenderSystem*  mRenderSystem;
 Critter::Node*          mNode;
 
 enum SinbadSectionID
 {
  SinbadLower = 0,
  SinbadUpper = 1
 };
 
 void setupPhysics()
 {
  
  // Create everything Quickly.
  mWorld = NxOgre::World::createWorld();
  mScene = mWorld->createScene();
  mRenderSystem = new Critter::RenderSystem(mScene, mSceneMgr);
  
  // Setup Animations.
  mRenderSystem->addAnimation("sinbad.mesh",SinbadLower, Critter::Enums::StockAnimationID_Idle, "IdleBase");
  mRenderSystem->addAnimation("sinbad.mesh",SinbadUpper, Critter::Enums::StockAnimationID_Idle, "IdleTop");

  mRenderSystem->addAnimation("sinbad.mesh",SinbadLower, Critter::Enums::StockAnimationID_Forward, "RunBase");
  mRenderSystem->addAnimation("sinbad.mesh",SinbadUpper, Critter::Enums::StockAnimationID_Forward, "RunTop");


  // Briefly setup Sinbad, like in the previous tutorial.
  mNode = mRenderSystem->createNode(); 
  mNode->createAndAttachEntity("sinbad.mesh");
  mNode->setPosition(0,2.5,0);
  mNode->setScale(0.5);
  
  // Start animating sinbad with the Idle pose.
  mNode->setAnimation(SinbadLower, Critter::Enums::StockAnimationID_Idle);
  mNode->setAnimation(SinbadUpper, Critter::Enums::StockAnimationID_Idle);

 }
 
 bool frameRenderingQueued(const FrameEvent& evt)
 {
  
  // Because our Node isn't part of a Body, or KinematicBody that regually listens to
  // timing events, we must manually update the animation ourselves.
  
  mNode->updateAnimations(evt.timeSinceLastFrame);
  

  // Advance NxOgre.
  mWorld->advance(evt.timeSinceLastFrame);
  return SdkSample::frameRenderingQueued(evt);
 }
 
 
 void sliderMoved(OgreBites::Slider* slider)
 {
  
  if (slider->getName() == "LowerAnimationSpeed")
  {
   mNode->setCurrentAnimationSpeed(SinbadLower, slider->getValue());
  }
  else if (slider->getName() == "UpperAnimationSpeed")
  {
   mNode->setCurrentAnimationSpeed(SinbadUpper, slider->getValue());
  }
  
 }
 
 void itemSelected(OgreBites::SelectMenu* menu)
 {
  size_t section = -1, index = 0;

  if (menu->getName() == "LowerAnimation")
   section = SinbadLower;
  else if (menu->getName() == "UpperAnimation")
   section = SinbadUpper;
  
  if (section == -1)
   return;
  
  if (menu->getSelectedItem() == "None")
   index = Critter::Enums::NO_ANIMATION;
  else if (menu->getSelectedItem() == "Idle")
   index = Critter::Enums::StockAnimationID_Idle;
  else if (menu->getSelectedItem() == "Run")
   index = Critter::Enums::StockAnimationID_Forward;
  
  mNode->setAnimation(section, index);
  
 }
 
 void stopPhysics()
 {
  // Delete the RenderSystem first before shutting down.
  delete mRenderSystem;

  // Then destroy Ogre.
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
  mCamera->lookAt(0,0,0);
  mCamera->setNearClipDistance(0.02f);
  mCamera->setFarClipDistance(1000.0f);
  mCameraMan->setTopSpeed(7.5);
  
  mTrayMgr->showCursor();
  setDragLook(true);
  

  Ogre::StringVector animnames;
  animnames.push_back("None");
  animnames.push_back("Idle");
  animnames.push_back("Run");
  animnames.push_back("Dance");
  
  mTrayMgr->createLabel(OgreBites::TL_RIGHT, "Upper", "Upper");
  mTrayMgr->createLongSelectMenu(OgreBites::TL_RIGHT, "UpperAnimation", "Animation", 150, animnames.size(), animnames)->selectItem(1, false);
  mTrayMgr->createLongSlider(OgreBites::TL_RIGHT, "UpperAnimationSpeed", "Speed", 150, 50, 0, 4.0f, 40)->setValue(1.0f,false);
  
  mTrayMgr->createLabel(OgreBites::TL_RIGHT, "LowerLabel", "Lower");
  mTrayMgr->createLongSelectMenu(OgreBites::TL_RIGHT, "LowerAnimation", "Animation", 150, animnames.size(), animnames)->selectItem(1, false); 
  mTrayMgr->createLongSlider(OgreBites::TL_RIGHT, "LowerAnimationSpeed", "Speed", 150, 50, 0, 4.0f, 40)->setValue(1.0f, false);
  
  setupPhysics();
  
 }
 
 void cleanupContent()
 {
  stopPhysics();
 }
 
 NxOgre1303()
 {
  mInfo["Title"] = "NxOgre 1303";
  mInfo["Description"] = "NxOgre 1303 - Critter - Animation";
  mInfo["Thumbnail"] = "thumb_skybox.png";
  mInfo["Category"] = "Physics";
  mInfo["Help"] = "Animation with the Node class";
 }

};
