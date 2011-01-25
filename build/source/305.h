/** 
    
    NxOgre Tutorials - 305 - Raycasting
    
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

class _OgreSampleClassExport NxOgre305 : public SdkSample
{
 
public:
 
 NxOgre::World*          mWorld;
 NxOgre::Scene*          mScene;
 Critter::RenderSystem*  mRenderSystem;
 unsigned int            mBodyId;
 OgreBites::Label*       mHitResultText;

 void setupPhysics()
 {
  
  createWorldSceneEtc();
  
  // Make 25 randomly positioned boxes.
  for (unsigned int i=0;i < 25;i++)
   makeBox(Vec3::random(-20,20,0.5,1,-20,20));
  
 }
 
 bool mouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
 {
  if (evt.state.buttonDown(OIS::MB_Left) == false)
   SdkSample::mouseReleased(evt, id);

  // Use Ogre and OgreBites to work out where from the cursor the ray should start and end.
  Ogre::Ray ogre_ray = mTrayMgr->getCursorRay(mCamera);

  // Convert it into an NxOgre ray.
  NxOgre::Ray ray;
  ray.mDirection.from(ogre_ray.getDirection());
  ray.mOrigin.from(ogre_ray.getOrigin());
  
  // Then cast it!
  RaycastHit hit = mScene->raycastClosestShape(ray, NxOgre::Enums::ShapesType_All);
  
  if (hit.mRigidBody)
  {
   std::string name = hit.mRigidBody->getName();
   if (name.length())
    mHitResultText->setCaption("Hit '" + hit.mRigidBody->getName() + "' at " + hit.mWorldImpact.to_s());
   else
    mHitResultText->setCaption("Hit something at " + hit.mWorldImpact.to_s());
   
  }
  else
  {
   mHitResultText->setCaption("Didn't find anything.");
  }
  
  return true;
 }
  
 Body* makeBox(const Matrix44& globalPose, const Vec3& initialVelocity = Vec3::ZERO)
 {
  
  Critter::BodyDescription bodyDescription;
  bodyDescription.mName = "Box#" + to_s(mBodyId++);  // Give it a proper name.
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
  
  mWorld->getRemoteDebugger()->connect();

  // Create the scene
  NxOgre::SceneDescription scene_description;
  scene_description.mGravity = NxOgre::Constants::MEAN_EARTH_GRAVITY;
  scene_description.mUseHardware = false;

  mScene = mWorld->createScene(scene_description);
  
  // Set default material properties
  mScene->getMaterial(0)->setAll(0.1f, 0.9f, 0.5f);
  
  // Plane creation
  RigidBodyDescription plane_geom_desc;
  plane_geom_desc.mName = "Ground Plane";
  
  mScene->createSceneGeometry(NxOgre::PlaneGeometryDescription(), Vec3::ZERO, plane_geom_desc);

  // Create the rendersystem.
  mRenderSystem = new Critter::RenderSystem(mScene, mSceneMgr);
  
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
  
  mHitResultText = mTrayMgr->createLabel(OgreBites::TL_RIGHT, "HitResult", "Click on something.", 400);

  setDragLook(true);
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

 NxOgre305() : mBodyId(0)
 {
  mInfo["Title"] = "NxOgre 305";
  mInfo["Description"] = "NxOgre 305 - Raycasting";
  mInfo["Thumbnail"] = "thumb_skybox.png";
  mInfo["Category"] = "Physics";
  mInfo["Help"] = "Click on something to get the name of the raycast";
 }

};
