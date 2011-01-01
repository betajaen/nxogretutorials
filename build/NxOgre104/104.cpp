/** 
    
    NxOgre Tutorials - 104 - RigidBodies
    
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

class _OgreSampleClassExport NxOgre104 : public SdkSample
{
 
public:
 
 NxOgre::World*          mWorld;
 NxOgre::Scene*          mScene;
 
 float                   mLastTimeStep;
 NxOgre::Material*       mDefaultMaterial;
 NxOgre::MeshManager*    mMeshManager;
 NxOgre::Mesh*           mBarrelMesh;
 NxOgre::Mesh*           mPlaypenMesh;
 Critter::RenderSystem*  mRenderSystem;
 Critter::KinematicBody* mOrb;
 Critter::KinematicBody* mOrb2;
 float                   mOrbDistanceLeft;
 NxOgre::Vec3            mOrbTarget, mOrbDirection;
  
 void setupPhysics()
 {
  
  // Create the world.
  mWorld = NxOgre::World::createWorld();
  
  // Allow NxOgre to use the Ogre resource system, via the Critter OgreResourceProtocol class.
  // - ResourceProtocols are normally owned by the ResourceSystem, so we don't have to delete it later.
  NxOgre::ResourceSystem::getSingleton()->openProtocol(new Critter::OgreResourceProtocol());
  
  // Connect to the Remote Debugger - if there is one.
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
  // The NxOgre Mesh Manager is responsible for loading and storing all Meshes (Triangle, Convex, Cloth, etc.)
  // - Let's get a copy of it's pointer
  mMeshManager = NxOgre::MeshManager::getSingleton();
  
  // Create our playpen, to contain our bodies.
  makePlaypen(Vec3(0,0.5,0));

  // Create a load of barrels.
  makeBarrel(Vec3(-0.5,1,0), Vec3(0,0,0));
  makeBarrel(Vec3(-0.5,2,0), Vec3(0,0,0));
  makeBarrel(Vec3(0.5,1,0), Vec3(0,0,0));
  makeBarrel(Vec3(0.5,2,0), Vec3(0,0,0));
  makeBarrel(Vec3(-0.5,1,-1), Vec3(0,0,0));
  makeBarrel(Vec3(-0.5,2,-1), Vec3(0,0,0));
  makeBarrel(Vec3(0.5,1,-1), Vec3(0,0,0));
  makeBarrel(Vec3(0.5,2,-1), Vec3(0,0,0));
  makeBarrel(Vec3(-0.5,1,1), Vec3(0,0,0));
  makeBarrel(Vec3(-0.5,2,1), Vec3(0,0,0));
  makeBarrel(Vec3(0.5,1,1), Vec3(0,0,0));
  makeBarrel(Vec3(0.5,2,1), Vec3(0,0,0));

  // And a bunch of boxes.
  makeBox(Vec3(-3,1,0), Vec3(0,0,0));
  makeBox(Vec3(-2,1,0), Vec3(0,0,0));
  makeBox(Vec3(-2.5,2,0), Vec3(0,0,0));
  makeBox(Vec3(3,1,0), Vec3(0,0,0));
  makeBox(Vec3(2,1,0), Vec3(0,0,0));
  makeBox(Vec3(2.5,2,0), Vec3(0,0,0));
 
  // And make our floating orb KinematicBody.
  mOrb = makeOrb(Vec3(0,2.5,0));
  mOrb2 = makeOrb(Vec3(0,2.5,0));
  
  // Calculate where we should put the orb.
  calculateNextOrbDirection(mOrb);

 }

 SceneGeometry* makePlaypen(const Matrix44& globalPose)
 {
  
  if (mPlaypenMesh == 0)
   mPlaypenMesh = NxOgre::MeshManager::getSingleton()->load("ogre://General/playpen.nxs", "playpen");
  
  SceneGeometry* scene_geom = mScene->createSceneGeometry(TriangleGeometryDescription(mPlaypenMesh), globalPose);
  mRenderSystem->createSceneNodeEntityPair("nxogre.playpen.mesh", Vec3(globalPose), Quat(globalPose));
  return scene_geom;
 }

 Body* makeContainer(const Matrix44& globalPose)
 {
  
  // Create a ShapeDescriptions container (NxOgre::vector<ShapeDescription*>).
  ShapeDescriptions shapes;
  
  // Each shape in the Actor/Body may have it's own seperate material, but the default one is good enough for now.
  NxOgre::MaterialIdentifier material = mDefaultMaterial->getIdentifier();
  
  // Our container should be hollow inside, to let things through it. 
  // - Convex shapes cannot have holes.
  // - Spheres, Capsules do have holes either.
  // - Triangle Meshes can only be used in StaticGeometries, and StaticGeometries do not move.
  // - Instead we make up the container using box shapes repesenting each side of the container, there are four
  //   sides, so four BoxDescriptions.
  
  BoxDescription side1(Vec3(3.784, 1.75, 0.1),  material, Vec3(0, 0, -0.975));
  BoxDescription side2(Vec3(3.784, 1.75, 0.1), material, Vec3(0, 0,  0.975));
  BoxDescription side3(Vec3(3.784, 0.1, 2),  material, Vec3(0, -0.85, 0));
  BoxDescription side4(Vec3(3.784, 0.1, 2), material, Vec3(0, 0.85, 0));
  
  // Then add them to the ShapeDescriptions.
  // - You can use pointers and create the BoxDescriptions via new. But you need to delete them afterwards for leaks.
  // - Careful using references! As with C++ they will be deleted when they are out of scope.
  shapes.push_back(&side1);
  shapes.push_back(&side1);
  shapes.push_back(&side2);
  shapes.push_back(&side3);
  
  // Create the container BodyDescription.
  BodyDescription description;
  description.mMass = 1000; // Make it pretty heavy, 1000kg.
  
  // Create the container using the "shapes" ShapeDescriptions as our Shape argument.
  Body* container = mRenderSystem->createBody(shapes, globalPose, "container.mesh", description);
  
  return container;
 }

 Body* makeBox(const Matrix44& globalPose, const Vec3& initialVelocity)
 {
  
  Critter::BodyDescription bodyDescription;
  bodyDescription.mMass = 40.0f;
  bodyDescription.mLinearVelocity = initialVelocity;
  
  Body* box = mRenderSystem->createBody(BoxDescription(1,1,1), globalPose, "cube.1m.mesh", bodyDescription);
  
  return box;
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

 KinematicBody* makeOrb(const Matrix44& globalPose)
 {
  KinematicBody* orb = mRenderSystem->createKinematicBody(SphereDescription(0.21f), globalPose, "nxogre.orb.mesh");
  Light* light = mSceneMgr->createLight();
  light->setType(Ogre::Light::LT_POINT);
  Ogre::ColourValue colour(65.0f / 255.0f, 105.0f / 255.0f, 225.0f / 255.0f);
  light->setDiffuseColour(colour);
  orb->getNode()->addMovableObject(light);
  return orb;
 }

 bool keyPressed(const OIS::KeyEvent& evt)
 {
  if (evt.key == OIS::KC_1)
   makeBarrel(Matrix44(Vec3(-4,4,0), Quat(0.707, Vec3(1,0,0))), Vec3(1,1,0));
  
  if (evt.key == OIS::KC_2)
   makeBox(Matrix44(Vec3(-4,5,0), Quat(0.707, Vec3(1,0,0))), Vec3(1,1,0));
  
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

 void calculateNextOrbDirection(KinematicBody* orb)
 {
  mOrbTarget = Vec3::random(-10,10,0.01,6,-10,10);
  mOrbDirection = mOrbTarget - orb->getGlobalPosition();
  mOrbDirection.normalise();
  mOrbDistanceLeft = orb->getGlobalPosition().distance(mOrbTarget);
 }

 bool frameRenderingQueued(const FrameEvent& evt)
 {
  
  // Move the first orb
  mOrb->moveGlobalPosition(mOrb->getGlobalPosition() + (mOrbDirection * 0.25f));

  // The other orb will just do the opposite of the first orb.
  mOrb2->moveGlobalPosition(mOrb2->getGlobalPosition() + (-mOrbDirection * 0.25f));
  mOrbDistanceLeft = mOrb->getGlobalPosition().distance(mOrbTarget);
  
  if (mOrbDistanceLeft <= 0.5f)
   calculateNextOrbDirection(mOrb);
  
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

 NxOgre104() : mPlaypenMesh(0), mBarrelMesh(0)
 {
  mInfo["Title"] = "NxOgre 104";
  mInfo["Description"] = "NxOgre 104 - Rigid Bodies";
  mInfo["Thumbnail"] = "thumb_skybox.png";
  mInfo["Category"] = "Physics";
  mInfo["Help"] = "Showing off three types of RigidBodies; StaticGeometries, Actor/Bodies, KinematicActors/KinematicBodies";
 }

};

SamplePlugin* sp;
Sample* s;

extern "C" _OgreSampleExport void dllStartPlugin()
{
	s = new NxOgre104;
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
