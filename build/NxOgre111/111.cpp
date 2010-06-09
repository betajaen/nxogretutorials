/** 
    
    NxOgre Tutorials - 111 - Dynamic CCD
    
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

class _OgreSampleClassExport NxOgre111 : public SdkSample
{
 
public:
 
 NxOgre::World*          mWorld;
 NxOgre::Scene*          mScene;
 float                   mLastTimeStep;
 NxOgre::Material*       mDefaultMaterial;
 Critter::RenderSystem*  mRenderSystem;
 Critter::Body*          mBody;
 NxOgre::Mesh*           mCCDMesh;

  
 void setupPhysics()
 {
  
  // Create the world.
  mWorld = NxOgre::World::createWorld();
  mWorld->getRemoteDebugger()->connect();
  
  //
  NxOgre::ResourceSystem::getSingleton()->openProtocol(new Critter::OgreResourceProtocol());
  
  // Enable CCD via the World. Normally this is off by default in PhysX. So we have to switch it on.
  mWorld->setCCDEnabled(true);
  
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
  
  mRenderSystem->setVisualisationMode(NxOgre::Enums::VisualDebugger_ShowAll);
  
  BodyDescription desc;
  desc.mMass = 40.0f;
  BoxDescription box;
  box.mSize.set(1,1,1);
  if (mCCDMesh == 0)
   makeCCDBox();
  box.mSkeleton = mCCDMesh;
  
  mBody = mRenderSystem->createBody(box, Vec3(10,10,10), "cube.1m.mesh", desc);


  makeStack(10);
 }
 
 void stopPhysics()
 {
  NxOgre::World::destroyWorld();
 }
 
 bool keyPressed(const OIS::KeyEvent& evt)
 {
  
  if (evt.key == OIS::KC_SPACE)
  {
   Vec3 force(mCamera->getDirection());
   force *= 50;
   mBody->setGlobalPosition(Vec3(mCamera->getPosition()));
   mBody->setLinearVelocity(force);
   //mBody->addForce(NxOgre::Vec3(0,10000,0));
  }

  return SdkSample::keyPressed(evt);
 }

 void makeCCDBox()
 {
  if (mCCDMesh == 0)
   mCCDMesh = NxOgre::MeshManager::getSingleton()->load("ogre://cube.1m.xsk", "cube.1m");
 }

 void makeStack(int size)
 {
  
  // Setup a BodyDescription.
  Critter::BodyDescription bodyDescription;
  bodyDescription.mMass = 40.0f; // Set the mass to 40kg.

  NxOgre::BoxDescription box;
  box.mSize.set(1,1,1);
  
  if (mCCDMesh == 0)
   makeCCDBox();
  
  box.mSkeleton = mCCDMesh;
  
	 const float cubeSize = 0.75f;
	 const float spacing = 0;
	 Vec3 pos(0,0,0);
	 float offset = -size * (cubeSize * 2.0f + spacing) * 0.5f;
	 while(size)
	 {
		 for(int i=0;i<size;i++)
		 {
			 pos.x = offset + (float)i * (cubeSize * 2.0f + spacing);
     mRenderSystem->createBody(box, pos, "cube.1m.mesh", bodyDescription);
		 }

		 offset += cubeSize;
		 pos.y += (cubeSize * 2.0f + spacing);
		 size--;
	 }
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

 NxOgre111() : mCCDMesh(0)
 {
  mInfo["Title"] = "NxOgre 111";
  mInfo["Description"] = "NxOgre 111 - Dynamic CCD";
  mInfo["Thumbnail"] = "thumb_skybox.png";
  mInfo["Category"] = "Physics";
  mInfo["Help"] = "Collision Detection with Scary Skeletons! WooooOOOoo!";
 }

};

SamplePlugin* sp;
Sample* s;

extern "C" _OgreSampleExport void dllStartPlugin()
{
	s = new NxOgre111;
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

#if 0

  NxOgre::ManualMesh mesh;
  mesh.begin(NxOgre::Enums::MeshType_Skeleton);
  
  float size = 2 * 0.5f;
  
  mesh.vertex( size, -size, -size);
  mesh.vertex( size, -size,  size);
  mesh.vertex( size,  size, -size);
  mesh.vertex( size,  size,  size);
  mesh.vertex(-size, -size, -size);
  mesh.vertex(-size, -size,  size);
  mesh.vertex(-size,  size, -size);
  mesh.vertex(-size,  size,  size);
  
  mesh.triangle(0,1,3);
  mesh.triangle(0,3,2);
  mesh.triangle(3,7,6);
  mesh.triangle(3,6,2);
  mesh.triangle(1,5,7);
  mesh.triangle(1,7,3);
  mesh.triangle(4,6,7);
  mesh.triangle(4,7,5);
  mesh.triangle(1,0,4);
  mesh.triangle(5,1,4);
  mesh.triangle(4,0,2);
  mesh.triangle(4,2,6);
  
  mCCDMesh = mesh.end(true, "file://test.xsk");
#endif