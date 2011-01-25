/** 
    
    NxOgre Tutorials - 1001 - Cloth
    
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

class _OgreSampleClassExport NxOgre1001 : public SdkSample
{
 
public:
  
 enum VertexMethod
 {
  VM_SetPosition,
  VM_AttachVertex,
  VM_AddForce
 };


 NxOgre::World*          mWorld;
 NxOgre::Scene*          mScene;
 float                   mLastTimeStep;
 NxOgre::MeshManager*    mMeshManager;
 NxOgre::Material*       mDefaultMaterial;
 Critter::RenderSystem*  mRenderSystem;
 Cloth*                  mCloth;
 NxOgre::Mesh*           mClothMesh;
 int                     mClothSelectedVertex;
 Ogre::SceneNode*        mClothSelectedVertexMarker;
 OgreBites::SelectMenu*  mClothSelectedMethodMenu;
 VertexMethod            mClothSelectedMethod;
 
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
  // mRenderSystem->setVisualisationMode(NxOgre::Enums::VisualDebugger_ShowAll);
  
  makeBox(Vec3(0,0.5,0.5));
  makeCloth(Vec3(0,4,0));
  
  mClothSelectedVertexMarker = mRenderSystem->createSceneNodeEntityPair("cube.mesh", Ogre::Vector3(0,-10,0));
  mClothSelectedVertexMarker->setScale(0.001, 0.001, 0.001);
  mClothSelectedVertexMarker->roll(Ogre::Degree(45));
  mClothSelectedVertexMarker->yaw(Ogre::Degree(45));
  mClothSelectedVertexMarker->pitch(Ogre::Degree(45));
 }

 void stopPhysics()
 {
  NxOgre::World::destroyWorld();
 }
 
 bool keyPressed(const OIS::KeyEvent& evt)
 {
  return SdkSample::keyPressed(evt);
 }

 Body* makeBox(const Matrix44& globalPose, const Vec3& initialVelocity = Vec3::ZERO)
 {
  
  Critter::BodyDescription bodyDescription;
  bodyDescription.mMass = 40.0f;
  bodyDescription.mLinearVelocity = initialVelocity;
  
  Body* box = mRenderSystem->createBody(BoxDescription(1,1,1), globalPose, "cube.1m.mesh", bodyDescription);

  return box;
 }

 void makeCloth(const Vec3& barPosition)
 {

  Vec3 pos = barPosition;
  Vec2 clothSize(8,4);

  pos.x -= clothSize.x * 0.5f;
  
  if (mClothMesh == 0)
   mClothMesh = NxOgre::MeshGenerator::makePlane(clothSize, 0.1, NxOgre::Enums::MeshType_Cloth, "file://rug.xcl");
  
  Vec3 holderPos = pos;
  holderPos.x += clothSize.x * 0.5f;;
  holderPos.y += 0.05f;
  holderPos.z -= 0.05f;
  SceneGeometry* geom = mScene->createSceneGeometry(BoxDescription(10, 0.1, 0.1), holderPos);
  
  SceneNode* geom_node = mRenderSystem->createSceneNodeEntityPair("cube.mesh", holderPos);
  geom_node->scale(0.1, 0.001, 0.001);
  
  NxOgre::ClothDescription desc;
  desc.mMesh = mClothMesh;
  desc.mThickness = 0.2;
  desc.mFriction = 0.5;
  //desc.mFlags |= NxOgre::Enums::ClothFlags_BendingResistance;
  //desc.mFlags |= NxOgre::Enums::ClothFlags_TwoWayCollisions;
  
  desc.mGlobalPose.set(pos);
  
  mCloth = mRenderSystem->createCloth(desc, "wales");
  mCloth->attachToShape((*geom->getShapes().begin()), NxOgre::Enums::ClothAttachmentFlags_Twoway);
 }

 bool mousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
 {
  if (evt.state.buttonDown(OIS::MB_Right))
  {
   
   if (mClothSelectedVertex != -1)
   {
    mCloth->freeVertex(mClothSelectedVertex);
    mClothSelectedVertex = -1;
    mClothSelectedVertexMarker->setPosition(0,-1,0);
   }
   
   Ogre::Ray ogre_ray = mTrayMgr->getCursorRay(mCamera);
   NxOgre::Ray ray;
   ray.mDirection.from(ogre_ray.getDirection());
   ray.mOrigin.from(ogre_ray.getOrigin());
   NxOgre::ClothRaycastHit result = mCloth->raycast(ray);
   
   if (result.mDidHit)
    mClothSelectedVertex = result.mVertexId;
   else
    mClothSelectedVertex = -1;
   
  }
  
  return SdkSample::mousePressed(evt, id);
 }

 bool mouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
 {
  
  if (mClothSelectedVertex != -1)
  {
   mCloth->freeVertex(mClothSelectedVertex);
   mClothSelectedVertex = -1;
   mClothSelectedVertexMarker->setPosition(0,-1,0);
  }
  
  return SdkSample::mouseReleased(evt, id);
 }

 bool mouseMoved(const OIS::MouseEvent& evt)
 {
  return SdkSample::mouseMoved(evt);
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
  
  mClothSelectedMethodMenu = mTrayMgr->createLongSelectMenu(OgreBites::TL_RIGHT, "MoveMethod", "Method", 300, 200, 5);
  mClothSelectedMethodMenu->addItem("setPosition");
  mClothSelectedMethodMenu->addItem("attachVertexToGlobalPosition");
  mClothSelectedMethodMenu->addItem("addForceAtVertex");
  
  setupPhysics();
  setDragLook(true);
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

  if (mClothSelectedVertex != -1)
  {
   
   Ogre::Ray ogre_ray = mTrayMgr->getCursorRay(mCamera);
   NxOgre::Ray ray;
   ray.mDirection.from(ogre_ray.getDirection());
   ray.mOrigin.from(ogre_ray.getOrigin());
   NxOgre::RaycastHit result = mScene->raycastClosestShape(ray, NxOgre::Enums::ShapesType_All);
   
   if (result.mDistance > 0 && result.mDistance < 100) // Between 0 and 100 metres is enough.
   {
    if (mClothSelectedMethod == VM_AttachVertex)
    {
     mCloth->attachVertexToGlobalPosition(mClothSelectedVertex, result.mWorldImpact);
    }
    else if (mClothSelectedMethod == VM_SetPosition)
    {
     mCloth->setPosition(result.mWorldImpact, mClothSelectedVertex);
    }
    else if (mClothSelectedMethod == VM_AddForce)
    {
     Vec3 force = mCloth->getPosition(mClothSelectedVertex) - result.mWorldImpact;
     force.normalise();
     mCloth->addForceAtVertex(-force * 100, mClothSelectedVertex);
    }

    mClothSelectedVertexMarker->setPosition(mCloth->getPosition(mClothSelectedVertex).as<Ogre::Vector3>());
   }
   
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


 void itemSelected(SelectMenu* menu)
 {
  if (menu != mClothSelectedMethodMenu)
   return;
  
  Ogre::String name = menu->getSelectedItem();
  
  if (name == "setPosition")
   mClothSelectedMethod = VM_SetPosition;
  else if(name == "attachVertexToGlobalPosition")
   mClothSelectedMethod = VM_AttachVertex;
  else if (name == "addForceAtVertex")
   mClothSelectedMethod = VM_AddForce;
  
 }

 NxOgre1001() : mClothMesh(0), mClothSelectedMethod(VM_SetPosition), mClothSelectedVertex(-1)
 {
  mInfo["Title"] = "NxOgre 1001";
  mInfo["Description"] = "NxOgre 1001 - Cloth";
  mInfo["Thumbnail"] = "thumb_skybox.png";
  mInfo["Category"] = "Physics";
  mInfo["Help"] = "Introduction to Cloth.";
 }

};
