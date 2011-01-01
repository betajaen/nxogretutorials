/** 
    
    NxOgre Tutorials - 110 - Collision Filtering
    
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

class _OgreSampleClassExport NxOgre110 : public SdkSample
{
 
public:
 
 NxOgre::World*          mWorld;
 NxOgre::Scene*          mScene;
 
 NxOgre::MeshManager*    mMeshManager;
 float                   mLastTimeStep;
 NxOgre::Material*       mDefaultMaterial;
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

  // Create the time controller
  

  // Get a copy of it's pointer
  mMeshManager = NxOgre::MeshManager::getSingleton();

  // Create the scene
  NxOgre::SceneDescription scene_description;
  scene_description.mGravity = Vec3(0,-20,0); // Up the gravity a bit!
  scene_description.mUseHardware = false;

  mScene = mWorld->createScene(scene_description);
  
  // Set default material properties
  mDefaultMaterial = mScene->getMaterial(0);
  mDefaultMaterial->setRestitution(0.1f);
  mDefaultMaterial->setDynamicFriction(0.3);        // A bit slippery
  mDefaultMaterial->setStaticFriction(0.2);
  


  mScene->setFilterOps(NxOgre::Enums::FilterOp_Or, NxOgre::Enums::FilterOp_Or, NxOgre::Enums::FilterOp_And);
  mScene->setFilterBool(true);
  
  GroupsMask zero_mask;
  zero_mask.zero();
  
  mScene->setFilterConstant0(zero_mask);
  mScene->setFilterConstant1(zero_mask);
  

  // Plane creation
  PlaneGeometryDescription ground_plane;
  ground_plane.mGroupsMask.bits0 = 0xffffffff;
  ground_plane.mGroupsMask.bits1 = 0xffffffff;   // Want everything to collide with the ground plane.
  ground_plane.mGroupsMask.bits2 = 0xffffffff;
  ground_plane.mGroupsMask.bits3 = 0xffffffff;


  mScene->createSceneGeometry(ground_plane);

  // Create the rendersystem.
  mRenderSystem = new Critter::RenderSystem(mScene, mSceneMgr);
  
  // mRenderSystem->setVisualisationMode(NxOgre::Enums::VisualDebugger_ShowAll);
  
#if 1
  makePlatform(Vec3(0,10,0));
  
  bool isNice = false;
  for (unsigned int i=0;i < 100;i++)
  {
   
   Vec3 pos = Vec3::random(-30,30,0, 3, -30,30);
   pos.y += 10;
   
   if (isNice)
     makeBox(pos);
   else
     makeNaughtyBox(pos);
   
   isNice = !isNice;
   
  }
#else
  
  for (unsigned int i=0;i < 10;i++)
  {
   if (i < 5)
    makeBox(Vec3(0,0.5+i,0));
   else
    makeNaughtyBox(Vec3(0,0.5+i,0));
  }
#endif
 }
 
 SceneGeometry* makePlatform(const Matrix44& globalPose)
 {
  
  ShapeDescriptions shapes;
  
  TriangleGeometryDescription platform_shape;
  platform_shape.mMesh = loadOrGetMesh("ogre://platform.nxs");

  platform_shape.mGroupsMask.bits0 = 0xffffffff;
  platform_shape.mGroupsMask.bits1 = 0xffffffff;   // Want everything to collide with the platform ring.
  platform_shape.mGroupsMask.bits2 = 0xffffffff;
  platform_shape.mGroupsMask.bits3 = 0xffffffff;

  TriangleGeometryDescription forcefield_shape;
  forcefield_shape.mMesh = loadOrGetMesh("ogre://forcefield.nxs");
  
  
  forcefield_shape.mGroupsMask.bits0 = 1;

  shapes.push_back(&platform_shape);
  shapes.push_back(&forcefield_shape);
  
  SceneGeometry* geom = mScene->createSceneGeometry(shapes, globalPose);
  Ogre::SceneNode* node = mRenderSystem->createSceneNodeEntityPair("nxogre.platform.mesh", Vec3(globalPose), Quat(globalPose));
  
  return geom;
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
  
  BoxDescription box_desc;
  box_desc.mSize.set(1,1,1);
  box_desc.mGroupsMask.bits0 = 1;
  
  Body* box = mRenderSystem->createBody(box_desc, globalPose, "cube.1m.mesh", bodyDescription);
  
  return box;
 }

 Body* makeNaughtyBox(const Matrix44& globalPose, const Vec3& initialVelocity = Vec3::ZERO)
 {
  
  Critter::BodyDescription bodyDescription;
  bodyDescription.mMass = 35.0f;
  bodyDescription.mLinearVelocity = initialVelocity + Vec3::random(-1,1,-1,1,-1,1);
  
  BoxDescription box_desc;
  box_desc.mSize.set(1,1,1);
  
  box_desc.mGroupsMask.bits1 = 1;

  Body* box = mRenderSystem->createBody(box_desc, globalPose, "cube.1m.mesh", bodyDescription);
  box->getNode()->getEntityAt(0)->setMaterialName("radioactive-cube-from-nxogre.org");

  return box;
 }


 bool keyPressed(const OIS::KeyEvent& evt)
 {
  if (evt.key == OIS::KC_1)
   makeBox(Matrix44(Vec3(-4,15,0), Quat(0.707, Vec3(1,0,0))), Vec3(10,5,0));
  
  if (evt.key == OIS::KC_2)
   makeNaughtyBox(Matrix44(Vec3(-4,15,0), Quat(0.707, Vec3(1,0,0))), Vec3(10,5,0));
  
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

 NxOgre110()
 {
  mInfo["Title"] = "NxOgre 110";
  mInfo["Description"] = "NxOgre 110 - Collision Filtering";
  mInfo["Thumbnail"] = "thumb_skybox.png";
  mInfo["Category"] = "Physics";
  mInfo["Help"] = "Filtering collisions.";
 }

};

SamplePlugin* sp;
Sample* s;

extern "C" _OgreSampleExport void dllStartPlugin()
{
	s = new NxOgre110;
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
