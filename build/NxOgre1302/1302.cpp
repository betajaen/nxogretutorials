/** 
    
    NxOgre Tutorials - 1302 - Nodes
    
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
#include "Critter.h"

using namespace Ogre;
using namespace OgreBites;

class _OgreSampleClassExport NxOgre1302 : public SdkSample
{
 
public:
 
 NxOgre::World*          mWorld;
 NxOgre::Scene*          mScene;
 Critter::RenderSystem*  mRenderSystem;
 Critter::Node*          mNode;

 
 void setupPhysics()
 {
  
  // Create everything Quickly.
  mWorld = NxOgre::World::createWorld();
  mScene = mWorld->createScene();
  mRenderSystem = new Critter::RenderSystem(mScene, mSceneMgr);
  
  // A Critter node, is a helper wrapper class for the Ogre::SceneNode class, 
  // used by Bodies, KinematicBodies, Characters, etc. It can handle attached
  // movable objects at ease, and even supports animation.
  
  mNode = mRenderSystem->createNode(); // Helper function, we can also do; new Node(mSceneMgr, mRenderSystem).
  
  // Going to create and attach an entity to this.
  mNode->createAndAttachEntity("sinbad.mesh");
  
  // Current the Camera (mCamera), is attached to another SceneNode.
  // A Critter Node, can automatically deattach it safely and then
  // attach it to itself; if you ask it to. SceneNodes require you
  // to do this yourself.
  
  mNode->addMovableObject(mCamera); // Automatically deattaches the camera, and attaches it to itself.
  mCamera->setPosition(0,5,-5);
  mCamera->lookAt(0,2,0);
  
  // Finally move the node up a bit.
  mNode->setPosition(0,2.5,0);
  // and then scale it down a a little.
  mNode->setScale(0.5);
  
 }

 void stopPhysics()
 {
  
  delete mNode; // (or mRenderSystem->destroyNode if you prefer). 
                // This will automatically delete the Sinbad entity, 
                // and move the Camera back to the root SceneNode.
  
  // Delete the RenderSystem first before shutting down.
  delete mRenderSystem;
  
  // Then destroy Ogre.
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
  mWorld->advance(evt.timeSinceLastFrame);
  return SdkSample::frameRenderingQueued(evt);
 }

 NxOgre1302()
 {
  mInfo["Title"] = "NxOgre 1302";
  mInfo["Description"] = "NxOgre 1302 - Critter - Nodes";
  mInfo["Thumbnail"] = "thumb_skybox.png";
  mInfo["Category"] = "Physics";
  mInfo["Help"] = "Introduction to the Node class";
 }

};

SamplePlugin* sp;
Sample* s;

extern "C" _OgreSampleExport void dllStartPlugin()
{
 s = new NxOgre1302;
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
