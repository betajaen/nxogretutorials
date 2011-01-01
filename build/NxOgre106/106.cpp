/** 
    
    NxOgre Tutorials - 106 - Sleeping
    
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

class _OgreSampleClassExport NxOgre106 : public SdkSample, public NxOgre::Callback
{
 
public:
 
 NxOgre::World*          mWorld;
 NxOgre::Scene*          mScene;
 
 float                   mLastTimeStep;
 NxOgre::Material*       mDefaultMaterial;
 Critter::RenderSystem*  mRenderSystem;
 NxOgre::Mesh*           mBarrelMesh;
 unsigned int            mNbParticles;
  
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
  mScene->setSleepCallback(this);
  
  // Set default material properties
  mDefaultMaterial = mScene->getMaterial(0);
  mDefaultMaterial->setRestitution(0.1f);
  mDefaultMaterial->setDynamicFriction(0.9);
  mDefaultMaterial->setStaticFriction(0.5);
  
  // The NxOgre Mesh Manager is responsible for loading and storing all Meshes (Triangle, Convex, Cloth, etc.)
  // - Let's get a copy of it's pointer
  NxOgre::MeshManager* mMeshManager = NxOgre::MeshManager::getSingleton();

  // Plane creation
  mScene->createSceneGeometry(NxOgre::PlaneGeometryDescription());

  // Create the rendersystem.
  mRenderSystem = new Critter::RenderSystem(mScene, mSceneMgr);
  
  // mRenderSystem->setVisualisationMode(NxOgre::Enums::VisualDebugger_ShowAll);
  // Setup a BodyDescription.
  Critter::BodyDescription bodyDescription;
  bodyDescription.mMass = 40.0f; // Set the mass to 40kg.
  
  makeBox(Vec3(-2,5,0), Vec3(0,0,1));
  
  mNbParticles = 0;
 }

 void stopPhysics()
 {
  NxOgre::World::destroyWorld();
 }
 
 void onSleep(const RigidBodyEventIterator& rigidbodies)
 {
  for (size_t i = 0; i < rigidbodies.size();i++)
  {
   RigidBody* rigidbody = rigidbodies[i];
   
   // Only want bodies.
   if(rigidbody->getRigidBodyType() != Critter::Enums::RigidBodyType_Body)
    continue;
   
   Body* body = static_cast<Body*>(rigidbody);
   
   Ogre::SceneNode* node = body->getNode()->getSceneNode();
   
   if (node->numChildren() == 0)
   {
    Ogre::SceneNode* childNode = body->getNode()->createChildSceneNode();
    childNode->setInheritOrientation(false);
    childNode->attachObject(mSceneMgr->createParticleSystem("zz" + NxOgre::to_s(mNbParticles++), "nxogre.zz"));
   }
   else
   {
    Ogre::ParticleSystem* particleSys = static_cast<Ogre::ParticleSystem*>(static_cast<Ogre::SceneNode*>(node->getChild(0))->getAttachedObject(0));
    particleSys->setEmitting(true);
   }
  }
 }

 void onWake(const RigidBodyEventIterator& rigidbodies)
 {
  for (size_t i = 0; i < rigidbodies.size();i++)
  {
   RigidBody* rigidbody = rigidbodies[i];
   
   // Only want bodies.
   if(rigidbody->getRigidBodyType() != Critter::Enums::RigidBodyType_Body)
    continue;
   
   Body* body = static_cast<Body*>(rigidbody);
   
   Critter::Node* node = body->getNode();
   
   if (node->numChildren() == 0)
    continue;
   
   Ogre::ParticleSystem* particleSys = node->getParticleSystemAt(0);
   particleSys->setEmitting(false);

  }
 }

 Body* makeBox(const Matrix44& globalPose, const Vec3& initialVelocity)
 {
  
  Critter::BodyDescription bodyDescription;
  bodyDescription.mMass = 40.0f;
  bodyDescription.mLinearVelocity = initialVelocity;
  
  Body* body = mRenderSystem->createBody(BoxDescription(1,1,1), globalPose, "cube.1m.mesh", bodyDescription);
  
  return body;
 }

 Body* makeBarrel(const Matrix44& globalPose, const Vec3& initialVelocity)
 {
  
  if (mBarrelMesh == 0)
   mBarrelMesh = NxOgre::MeshManager::getSingleton()->load("ogre://barrel.nxs", "barrel");
  
  Critter::BodyDescription bodyDescription;
  bodyDescription.mMass = 40.0f;
  bodyDescription.mLinearVelocity = initialVelocity;
  
  Body* barrel = mRenderSystem->createBody(ConvexDescription(mBarrelMesh), globalPose, "barrel.mesh", bodyDescription);
  
  return barrel;
 }

 bool keyPressed(const OIS::KeyEvent& evt)
 {
  
  if (evt.key == OIS::KC_1)
   makeBox(Matrix44(Vec3(-4,5,0), Quat(0.707, Vec3(1,0,0))), Vec3(1,1,0));
  
  if (evt.key == OIS::KC_2)
   makeBarrel(Matrix44(Vec3(-4,7,0), Quat(0.707, Vec3(1,0,0))), Vec3(1,1,0));

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

  // Don't let the camera go underground.
  if (mCamera->getPosition().y < 0.5f)
  {
   Ogre::Vector3 pos = mCamera->getPosition();
   pos.y = 0.5f;
   mCamera->setPosition(pos);
  }

  return SdkSample::frameRenderingQueued(evt);
 }

 NxOgre106()
 {
  mInfo["Title"] = "NxOgre 106";
  mInfo["Description"] = "NxOgre 106 - Sleeping";
  mInfo["Thumbnail"] = "thumb_skybox.png";
  mInfo["Category"] = "Physics";
  mInfo["Help"] = "Putting Actors and Bodies to sleep.";
 }

};

SamplePlugin* sp;
Sample* s;

extern "C" _OgreSampleExport void dllStartPlugin()
{
 s = new NxOgre106;
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
