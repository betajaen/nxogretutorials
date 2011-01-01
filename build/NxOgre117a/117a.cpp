/** 
    
    NxOgre Tutorials - 117 - Dynamic CCD
    
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
#include "OGRE/Terrain/OgreTerrain.h"
#include "OGRE/Terrain/OgreTerrainGroup.h"

#include "NxOgre.h"
#include "critter.h"

using namespace Ogre;
using namespace OgreBites;
using namespace NxOgre;
using namespace Critter;

class _OgreSampleClassExport NxOgre117 : public SdkSample
{
 
public:
 
 NxOgre::World*               mWorld;
 NxOgre::Scene*               mScene;
 NxOgre::Material*            mDefaultMaterial;
 Critter::RenderSystem*       mRenderSystem;
 Light*                       mLight;
 Ogre::TerrainGlobalOptions*  mTerrainGlobals;
 Ogre::TerrainGroup*          mTerrainGroup;
 
 void setupPhysics()
 {
  
  // Create the world.
  mWorld = NxOgre::World::createWorld();
  mWorld->getRemoteDebugger()->connect();
  
  //
  NxOgre::ResourceSystem::getSingleton()->openProtocol(new Critter::OgreResourceProtocol());
  
  // Create the scene
  NxOgre::SceneDescription scene_description;
  scene_description.mGravity = NxOgre::Constants::MEAN_EARTH_GRAVITY;
  scene_description.mUseHardware = false;

  mScene = mWorld->createScene(scene_description);
  
  // Set default material properties
  mScene->getMaterial(0)->setAll(0.1f, 0.9f, 0.5f);
  
  // Create the rendersystem.
  mRenderSystem = new Critter::RenderSystem(mScene, mSceneMgr);
  
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
  
  ColourValue background = ColourValue(135.0f/255.f, 206.f/255.f, 235.f/255.f);
  mViewport->setBackgroundColour(background);
  

  // add a bright light above the scene
  mLight = mSceneMgr->createLight();
  mLight->setType(Light::LT_POINT);
  mLight->setPosition(-10, 40, 20);
  mLight->setSpecularColour(ColourValue::White);
  
  mCamera->setPosition(10,10,10);
  mCamera->lookAt(0,0,0);
  mCamera->setNearClipDistance(0.02f);
  mCamera->setFarClipDistance(1000.0f);
  mCameraMan->setTopSpeed(7.5);
  
  mSceneMgr->setSkyBox(true, "Examples/CloudyNoonSkyBox");
  
  setupPhysics();
  setupTerrain();
  setupHeightFields();
 }
 
 void cleanupContent()
 {
  OGRE_DELETE mTerrainGroup;
  OGRE_DELETE mTerrainGlobals;
  stopPhysics();
 }
 
 void setupTerrain()
 {
  mTerrainGlobals = OGRE_NEW TerrainGlobalOptions();
  mTerrainGroup = OGRE_NEW TerrainGroup(mSceneMgr, Terrain::ALIGN_X_Z, 513, 10000);
  mTerrainGroup->setOrigin(Ogre::Vector3(0,-150,0));

  // Configure global
  mTerrainGlobals->setMaxPixelError(8);
  // testing composite map
  mTerrainGlobals->setCompositeMapDistance(3000);
  //mTerrainGlobals->setUseRayBoxDistanceCalculation(true);
  //mTerrainGlobals->getDefaultMaterialGenerator()->setDebugLevel(1);
  //mTerrainGlobals->setLightMapSize(256);

  //matProfile->setLightmapEnabled(false);
  // Important to set these so that the terrain knows what to use for derived (non-realtime) data
  mTerrainGlobals->setLightMapDirection(mLight->getDerivedDirection());
  mTerrainGlobals->setCompositeMapAmbient(mSceneMgr->getAmbientLight());
  //mTerrainGlobals->setCompositeMapAmbient(ColourValue::Red);
  mTerrainGlobals->setCompositeMapDiffuse(mLight->getDiffuseColour());

  // Configure default import settings for if we use imported image
  Terrain::ImportData& defaultimp = mTerrainGroup->getDefaultImportSettings();
  defaultimp.terrainSize = 513;
  defaultimp.worldSize = 1000;
  defaultimp.inputScale = 100;
  defaultimp.minBatchSize = 33;
  defaultimp.maxBatchSize = 65;
  // textures
  defaultimp.layerList.resize(3);
  defaultimp.layerList[0].worldSize = 100;
  defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_diffusespecular.dds");
  defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_normalheight.dds");
  defaultimp.layerList[1].worldSize = 30;
  defaultimp.layerList[1].textureNames.push_back("grass_green-01_diffusespecular.dds");
  defaultimp.layerList[1].textureNames.push_back("grass_green-01_normalheight.dds");
  defaultimp.layerList[2].worldSize = 200;
  defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_diffusespecular.dds");
  defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_normalheight.dds");

  Image img;
  img.load("terrain.png", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
  mTerrainGroup->defineTerrain(0, 0, &img);

  mTerrainGroup->loadAllTerrains(true);
  TerrainGroup::TerrainIterator ti = mTerrainGroup->getTerrainIterator();
  while(ti.hasMoreElements())
  {
      Terrain* t = ti.getNext()->instance;
      initBlendMaps(t);
  }
  mTerrainGroup->freeTemporaryResources();
 }

void initBlendMaps(Terrain* terrain)
    {
        TerrainLayerBlendMap* blendMap0 = terrain->getLayerBlendMap(1);
        TerrainLayerBlendMap* blendMap1 = terrain->getLayerBlendMap(2);
        Ogre::Real minHeight0 = 70;
        Ogre::Real fadeDist0 = 40;
        Ogre::Real minHeight1 = 70;
        Ogre::Real fadeDist1 = 15;
        float* pBlend1 = blendMap1->getBlendPointer();
        for (Ogre::uint16 y = 0; y < terrain->getLayerBlendMapSize(); ++y)
        {
            for (Ogre::uint16 x = 0; x < terrain->getLayerBlendMapSize(); ++x)
            {
                Ogre::Real tx, ty;

                blendMap0->convertImageToTerrainSpace(x, y, &tx, &ty);
                Ogre::Real height = terrain->getHeightAtTerrainPosition(tx, ty);
                Ogre::Real val = (height - minHeight0) / fadeDist0;
                val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
                //*pBlend0++ = val;

                val = (height - minHeight1) / fadeDist1;
                val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
                *pBlend1++ = val;


            }
        }
        blendMap0->dirty();
        blendMap1->dirty();
        //blendMap0->loadImage("blendmap1.png", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        blendMap0->update();
        blendMap1->update();

        // set up a colour map
        /*
        if (!terrain->getGlobalColourMapEnabled())
        {
            terrain->setGlobalColourMapEnabled(true);
            Image colourMap;
            colourMap.load("testcolourmap.jpg", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
            terrain->getGlobalColourMap()->loadImage(colourMap);
        }
        */

    }

 void setupHeightFields()
 {
  TerrainGroup::TerrainIterator ti = mTerrainGroup->getTerrainIterator();
  while(ti.hasMoreElements())
  {
   Terrain* t = ti.getNext()->instance;
   mRenderSystem->createTerrain(t);
  }
 }
 
 bool frameRenderingQueued(const FrameEvent& evt)
 {
  // Advance NxOgre.
  mWorld->advance(evt.timeSinceLastFrame);
  return SdkSample::frameRenderingQueued(evt);
 }
 
 void setupControls()
 {
  mTrayMgr->showCursor();
 }

 NxOgre117()
  : mTerrainGroup(0)
  , mTerrainGlobals(0)
 {
  mInfo["Title"] = "NxOgre 117";
  mInfo["Description"] = "NxOgre 117 - Heightfields with Ogre Terrain";
  mInfo["Thumbnail"] = "thumb_skybox.png";
  mInfo["Category"] = "Physics";
  mInfo["Help"] = "Heightfields with Ogre Terrain";
 }

};

SamplePlugin* sp;
Sample* s;

extern "C" _OgreSampleExport void dllStartPlugin()
{
	s = new NxOgre117;
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
