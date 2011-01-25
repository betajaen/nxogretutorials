/** 
    
    NxOgre Tutorials - 102 - External Resources and Meshes
    
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

class _OgreSampleClassExport NxOgre102 : public SdkSample
{
 
public:
 
 NxOgre::World*          mWorld;
 NxOgre::Scene*          mScene;
 float                   mLastTimeStep;
 NxOgre::Material*       mDefaultMaterial;
 NxOgre::MeshManager*    mMeshManager;
 NxOgre::Mesh*           mBarrelMesh;
 Critter::RenderSystem*  mRenderSystem;
  
 void setupPhysics()
 {
  
  // Create the world.
  mWorld = NxOgre::World::createWorld();
  
  // Allow NxOgre to use the Ogre resource system, via the Critter OgreResourceProtocol class.
  // - ResourceProtocols are normally owned by the ResourceSystem, so we don't have to delete it later.
  NxOgre::ResourceSystem::getSingleton()->openProtocol(new Critter::OgreResourceProtocol());
  
  // Connect to the Remote Debugger - if there is one.
  mWorld->getRemoteDebugger()->connect();

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
  // The NxOgre Mesh Manager is responsible for loading and storing all Meshes (Triangle, Convex, Cloth, etc.)
  // - Let's get a copy of it's pointer
  NxOgre::MeshManager* mMeshManager = NxOgre::MeshManager::getSingleton();

  // First off, let's load the Convex Mesh "barrel" through the Ogre resource system, we setup earlier.
  // - The filename is "barrel.nxs" and it's somewhere in the OgreSamples media folder under the "General" ResourceGroup.
  // - We shall call it "barrel".
  mBarrelMesh = mMeshManager->load("ogre://General/barrel.nxs", "barrel");

  // Next create a Body Description for our Bodies with barrels. It's going to be used many times
  // So, let's create it outside the for loop for optimisation reasons.
  Critter::BodyDescription bodyDescription;
  bodyDescription.mMass = 40.0f;  // Set a mass of 40kg.
  
  // 10 times, create a Body with a ConvexDescription using our barrel we loaded, stack each body on top of
  // each other in the air, and use the "barrel.mesh" for rendering.
  for (unsigned int i=0;i < 10;i++)
   mRenderSystem->createBody(NxOgre::ConvexDescription(mBarrelMesh), NxOgre::Vec3(0,45+(i*2),0), "barrel.mesh", bodyDescription);
  
  // Next up, let's load a Triangle Mesh "island.nxs" through the Ogre resource system.
  // - It's the general resource group, so we can omit that from the path. (NxOgre is clever enough to know what you mean)
  // - Call it "island".
  NxOgre::Mesh* island = mMeshManager->load("ogre://island.nxs", "island");
  
  // Remember TriangleMeshes can only be given into SceneGeometries, and NOT actors or bodies.
  // - We create the SceneGeometry through the Scene, and not Critter.
  // - We don't need a RigidBodyDescription for this, the defaults are fine.
  // - The visualisation is setup in the next few lines.
  mScene->createSceneGeometry(NxOgre::TriangleGeometryDescription(island), NxOgre::Vec3::ZERO);
  
  // The visualisation.
  SceneNode* islandNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(Vector3(0,0,0));
  islandNode->attachObject(mSceneMgr->createEntity("island", "island.mesh"));
  islandNode->setScale(0.3,0.3,0.3);
  
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

 NxOgre102() : mBarrelMesh(0)
 {
  mInfo["Title"] = "NxOgre 102";
  mInfo["Description"] = "NxOgre 102 - External Resources and Meshes";
  mInfo["Thumbnail"] = "thumb_skybox.png";
  mInfo["Category"] = "Physics";
  mInfo["Help"] = "Using the NxOgre Resource system and loading in meshes.";
 }

};
