/** 
    
    NxOgre Tutorials - 113 - Rigidbodies and Cloth
    
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

class _OgreSampleClassExport NxOgre113 : public SdkSample
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
 NxOgre::Mesh*           mFlagMesh;
 NxOgre::vector<Cloth*>  mCloths;
 float                   mMaxWind;
 OgreBites::Slider*      mWindSlider;
 
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
  mScene->getMaterial(0)->setAll(0.1, 0.9, 0.5);
  
  // Get a copy of it's pointer
  mMeshManager = NxOgre::MeshManager::getSingleton();

  // Plane creation
  mScene->createSceneGeometry(NxOgre::PlaneGeometryDescription());

  // Create the rendersystem.
  mRenderSystem = new Critter::RenderSystem(mScene, mSceneMgr);
  //mRenderSystem->setVisualisationMode(NxOgre::Enums::VisualDebugger_ShowAll);
  
  makeBox(Vec3(0,0.5,4));
  makeBox(Vec3(0,1.5,4));
  makeBox(Vec3(0,2.5,4));
  mBody = makeBox(Vec3(0,3.5,4));

  makeFlag(Vec3(-8,10,2), "germany");
  makeFlag(Vec3(-4,10,1), "uk");
  makeFlag(Vec3(0,10,0), "wales");
  makeFlag(Vec3(4,10,1), "usa");
  makeFlag(Vec3(8,10,2), "guernsey");
  
  mMaxWind = 10.0f;
  mWindSlider = mTrayMgr->createLongSlider(OgreBites::TL_RIGHT, "Wind", "Wind", 300, 100, 100, 0, 100, 100);
  mWindSlider->setValue(mMaxWind, false);
  
 }

 void stopPhysics()
 {
  NxOgre::World::destroyWorld();
 }
 
 Body* makeBox(const Matrix44& globalPose, const Vec3& initialVelocity = Vec3::ZERO)
 {
  
  Critter::BodyDescription bodyDescription;
  bodyDescription.mMass = 40.0f;
  bodyDescription.mLinearVelocity = initialVelocity;
  
  Body* box = mRenderSystem->createBody(BoxDescription(1,1,1), globalPose, "cube.1m.mesh", bodyDescription);

  return box;
 }

 Cloth* makeFlag(const Vec3& pos, const std::string& flag_image)
 {
  // Make the flag pole.
  Vec3 polePos = pos;
  polePos.y *= 0.5f;
  SceneGeometry* geom = mScene->createSceneGeometry(CapsuleDescription(0.15, 10), polePos);
  SceneNode* geom_node = mRenderSystem->createSceneNodeEntityPair("cube.mesh", polePos);
  geom_node->scale(0.0015, 0.10, 0.0015);
  
  // Make the flag
  if (mFlagMesh == 0)
   mFlagMesh = NxOgre::MeshGenerator::makePlane(Vec2(4,2), 0.1, NxOgre::Enums::MeshType_Cloth, "file://flag.xcl");
  
  // Make the cloth for the flag.
  NxOgre::ClothDescription desc;
  desc.mMesh = mFlagMesh;
  desc.mThickness = 0.2;
  desc.mFriction = 0.5;
  desc.mFlags |= NxOgre::Enums::ClothFlags_BendingResistance;
  desc.mFlags |= NxOgre::Enums::ClothFlags_TwoWayCollisions;
  desc.mGlobalPose.set(pos);
  desc.mGlobalPose.set(-NxOgre::Math::HalfPi, NxOgre::Enums::X);
  Cloth* cloth = mRenderSystem->createCloth(desc, flag_image);

  // Attach our flag to our flag pole, as the edge of the flag is inside the flag pole, the flag will stay attached.
  cloth->attachToShape((*geom->getShapes().begin()), 0);
  mCloths.push_back(cloth);
  return cloth;
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
  setDragLook(true);
 }
 
 void cleanupContent()
 {
  stopPhysics();
 }
 
 bool keyPressed(const OIS::KeyEvent& evt)
 {
  
  if (evt.key == OIS::KC_SPACE)
  {
   Vec3 force(mCamera->getDirection());
   force *= 50;
   mBody->setGlobalPosition(Vec3(mCamera->getPosition()));
   mBody->setLinearVelocity(force);
  }

  return SdkSample::keyPressed(evt);
 }
 
 bool frameRenderingQueued(const FrameEvent& evt)
 {
  // Advance NxOgre.
  mWorld->advance(evt.timeSinceLastFrame);
  
  mLastTimeStep = mScene->getTimeStep().getModified();

  for(NxOgre::vector_iterator<Cloth*> it = mCloths.elements(); it != it.end(); it++)
  {
   Vec3 wind = Vec3::random(-mMaxWind, mMaxWind, -mMaxWind, mMaxWind, -mMaxWind, mMaxWind);
   wind.x += 2.5f * mMaxWind;
   (*it)->setWindAcceleration(wind);
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
  
  if (slider == mWindSlider)
   mMaxWind = slider->getValue();
  
 }

 NxOgre113() : mFlagMesh(0)
 {
  mInfo["Title"] = "NxOgre 113";
  mInfo["Description"] = "NxOgre 113 - Rigidbodies and Cloth";
  mInfo["Thumbnail"] = "thumb_skybox.png";
  mInfo["Category"] = "Physics";
  mInfo["Help"] = "Rigidbodies and cloth.";
 }

};

SamplePlugin* sp;
Sample* s;

extern "C" _OgreSampleExport void dllStartPlugin()
{
	s = new NxOgre113;
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
