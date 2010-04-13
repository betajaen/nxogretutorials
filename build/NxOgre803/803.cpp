#include "803.h"
#include "NxOgre.h"
#include "critter.h"

using namespace NxOgre;

void NxOgre803::setupPhysics()
{
 
 // Create the world.
 mWorld = NxOgre::World::createWorld();
 
 // Create the time controller
 mTimeController = NxOgre::TimeController::getSingleton();

 // Create the scene
 NxOgre::SceneDescription scene_description;
 scene_description.mGravity = NxOgre::SceneDescription::MEAN_EARTH_GRAVITY;
 scene_description.mUseHardware = false;

 mPriScene = mWorld->createScene(scene_description, Enums::SceneType_Prinicipal);
 mAuxScene = mWorld->createScene(scene_description, Enums::SceneType_Auxiliary);
 
 // Planes creation
 mPriScene->createSceneGeometry(NxOgre::PlaneGeometryDescription());
 mAuxScene->createSceneGeometry(NxOgre::PlaneGeometryDescription());

 // Create the rendersystem.
 mPriRenderSystem = new Critter::RenderSystem(mPriScene, mSceneMgr);
 mAuxRenderSystem = new Critter::RenderSystem(mAuxScene, mSceneMgr);
 
 // Set up tutorial specific bits.
 setupTutorial();
}

void NxOgre803::stopPhysics()
{
 NxOgre::World::destroyWorld();
}

void NxOgre803::setupTutorial()
{
 
}

bool NxOgre803::keyPressed(const OIS::KeyEvent& evt)
{
 return SdkSample::keyPressed(evt);
}