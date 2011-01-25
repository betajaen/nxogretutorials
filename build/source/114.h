/** 
    
    NxOgre Tutorials - 114 - Rigidbodies and Fluids
    
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

class _OgreSampleClassExport NxOgre114 : public SdkSample
{
 
public:
 
 NxOgre::World*          mWorld;
 NxOgre::Scene*          mScene;
 
 float                   mLastTimeStep;
 NxOgre::MeshManager*    mMeshManager;
 NxOgre::Material*       mDefaultMaterial;
 Critter::RenderSystem*  mRenderSystem;
 Critter::Body*          mBody;
 Fluid*                  mFluid;
 FluidEmitter*           mEmitter;

  
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
  mDefaultMaterial = mScene->getMaterial(0);
  mDefaultMaterial->setRestitution(0.1f);
  mDefaultMaterial->setDynamicFriction(0.9);
  mDefaultMaterial->setStaticFriction(0.5);
  
  // Get a copy of it's pointer
  mMeshManager = NxOgre::MeshManager::getSingleton();

  // Plane creation
  mScene->createSceneGeometry(NxOgre::PlaneGeometryDescription());

  // Create the rendersystem.
  mRenderSystem = new Critter::RenderSystem(mScene, mSceneMgr);
  
  //// mRenderSystem->setVisualisationMode(NxOgre::Enums::VisualDebugger_ShowAll);
  
  // Create our playpen, to contain our bodies and fluids.
  makePlaypen(Vec3(0,0.5,0));
  
  makeBox(Vec3(0,5,0));
  
  makeFluid();
  
 }

 void stopPhysics()
 {
  NxOgre::World::destroyWorld();
 }
 
 bool keyPressed(const OIS::KeyEvent& evt)
 {
  return SdkSample::keyPressed(evt);
 }

 void makeFluid()
 {
  NxOgre::FluidDescription desc;
  desc.mMaxParticles = 2500;
  desc.mKernelRadiusMultiplier = 2.0f;
  desc.mRestParticlesPerMetre = 7.0f;
  desc.mMotionLimitMultiplier = 3.0f;
  desc.mPacketSizeMultiplier = 8;
  desc.mCollisionDistanceMultiplier = 0.1f;
  desc.mStiffness = 50.0f;
  desc.mViscosity = 10.0f;
  desc.mRestDensity = 1000.0f;
  desc.mSimulationMethod = NxOgre::Enums::FluidSimulationMethod_SPH;
  //desc.mFlags |= Enums::FluidFlags_Hardware;
  //desc.mFlags |= NxOgre::Enums::FluidFlags_DisableGravity;
   
  mFluid = mRenderSystem->createFluid(desc, "Gas", Critter::Enums::FluidType_OgreParticle);
  
  FluidEmitterDescription edesc;
  edesc.mPose.set(0, 3, 0);
  edesc.mShape = NxOgre::Enums::FluidEmitterShape_Rectangular;
  edesc.mParticleLifetime = 1.0;
  edesc.mRate = 50;
  edesc.mFluidSpeed = 1;
  edesc.mType = NxOgre::Enums::FluidEmitterType_FlowRate;
  edesc.mReplusionCoefficient = 0.8f;
  edesc.mDimensionX = 0.1f;
  edesc.mDimensionY = 0.1f;
  
  mEmitter = mFluid->createEmitter(edesc);
 }

 SceneGeometry* makePlaypen(const Matrix44& globalPose)
 {
  SceneGeometry* scene_geom = mScene->createSceneGeometry(TriangleGeometryDescription(loadOrGetMesh("ogre://General/playpen.nxs")), globalPose);
  mRenderSystem->createSceneNodeEntityPair("nxogre.playpen.mesh", Vec3(globalPose), Quat(globalPose));
  return scene_geom;
 }

 NxOgre::Mesh* loadOrGetMesh(const Path& path)
 {
  std::string name = path.getFilenameOnly();
  if (mMeshManager->hasMesh(name) == false)
   return mMeshManager->load(path, name);
  else
   return mMeshManager->getByName(name);
 }

 Body* makeBox(const Matrix44& globalPose, const Vec3& initialVelocity = Vec3::ZERO)
 {
  
  Critter::BodyDescription bodyDescription;
  bodyDescription.mMass = 40.0f;
  bodyDescription.mLinearVelocity = initialVelocity;
  
  Body* box = mRenderSystem->createBody(BoxDescription(1,1,1), globalPose, "cube.1m.mesh", bodyDescription);
  
  return box;
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

 NxOgre114()
 {
  mInfo["Title"] = "NxOgre 114";
  mInfo["Description"] = "NxOgre 114 - Rigidbodies and Fluids";
  mInfo["Thumbnail"] = "thumb_skybox.png";
  mInfo["Category"] = "Physics";
  mInfo["Help"] = "Rigidbodies and fluids.";
 }

};
