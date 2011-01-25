/** 
    
    NxOgre Tutorials - Test - Box on a Plane
    
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

#define MaxStrength 1e7f

class _OgreSampleClassExport NxOgreTest : public SdkSample
{
 
public:
 
 NxOgre::World*          mWorld;
 NxOgre::Scene*          mScene;
 float                    mLastTimeStep;
 NxOgre::Material*       mDefaultMaterial;
 Critter::RenderSystem*  mRenderSystem;
 Critter::Body*          mBody;

 NxOgre::map<Ogre::String, NxOgre::Capsule*, NxOgre::GC::NoGarbageCollection> mCapsules;
  
 void setupPhysics()
 {
  
  // Create the world.
  mWorld = NxOgre::World::createWorld();
  
  mWorld->getRemoteDebugger()->connect();

  // Create the scene
  NxOgre::SceneDescription scene_description;
  //scene_description.mGravity = NxOgre::Constants::MEAN_EARTH_GRAVITY;
  scene_description.mUseHardware = false;

  mScene = mWorld->createScene(scene_description);
  
  mRenderSystem = new Critter::RenderSystem(mScene, mSceneMgr);

  NxOgre::VisualDebuggerDescription desc;
  desc.showDebug();
  mRenderSystem->createVisualDebugger(desc);
  
  NxOgre::vector<Ogre::String> names;
  names.push_back("baldrick");
  names.push_back("edmund");
  names.push_back("percy");
  names.push_back("melchett");
  
  float z = 5;
  for (NxOgre::vector_iterator<Ogre::String> name = names.elements(); name != name.end(); name++)
  {
   NxOgre::CapsuleDescription cap_desc;
   cap_desc.mHeight = 2;
   cap_desc.mRadius = 0.2f;

   Critter::BodyDescription b_desc;
   b_desc.mName = (*name);
   b_desc.mMass = 100.0f;
   
   NxOgre::Matrix44 pose(NxOgre::Vec3(0,z,0), NxOgre::Quat(1,0,0,0));
   z += cap_desc.mHeight + 0.5;
   
   Critter::Body* body = mRenderSystem->createBody(cap_desc, pose, b_desc);
   NxOgre::Shape* shape = body->getShape(0);
   std::cout << "Shape type is : " << shape->getShapeFunctionType() << "\n";
   if (shape->getShapeFunctionType() == NxOgre::Enums::ShapeFunctionType_Capsule)
   {
    std::cout << "Inserting " << (*name) << "\n";
    mCapsules.insert( (*name), static_cast<NxOgre::Capsule*>(shape) );
   }
   
  }
  
 }

 void stopPhysics()
 {
  NxOgre::World::destroyWorld();
 }
 
 bool keyPressed(const OIS::KeyEvent& evt)
 {
  
  if (evt.key == OIS::KC_SPACE)
  {
   std::cout << "Space pressed\n";
   for (NxOgre::map_iterator<Ogre::String, NxOgre::Capsule*> capsule = mCapsules.elements(); capsule != capsule.end(); capsule++)
   {
    capsule->setHeight(NxOgre::Math::random(1, 5));
    capsule->setRadius(NxOgre::Math::random(0.25, 2));
    std::cout << capsule->getRigidBody()->getName() << " has now been changed!\n";
   }
   
  }
  
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
  
  mLastTimeStep = mScene->getTimeStep().getModified();

  // Don't let the camera go underground.
  if (mCamera->getPosition().y < 0.5f)
  {
   Ogre::Vector3 pos = mCamera->getPosition();
   pos.y = 0.5f;
   mCamera->setPosition(pos);
  }

  return SdkSample::frameRenderingQueued(evt);
 }

 NxOgreTest()
 {
  mInfo["Title"] = "NxOgre Test";
  mInfo["Description"] = "NxOgre Test";
  mInfo["Thumbnail"] = "thumb_skybox.png";
  mInfo["Category"] = "Physics";
  mInfo["Help"] = "Testing code.";
 }

};
