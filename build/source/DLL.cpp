#include "OGRE/SdkSample.h"
#include "OGRE/SamplePlugin.h"
#include "Test.h"
#include "101.h"
#include "102.h"
#include "103.h"
#include "104.h"
#include "105.h"
#include "106.h"
#include "107.h"
#include "108.h"
#include "109.h"
#include "110.h"
#include "111.h"
#include "113.h"
#include "114.h"
#include "117.h"
#include "303.h"
#include "304.h"
#include "304a.h"
#include "305.h"
#include "701.h"
#include "702.h"
#include "1001.h"
#include "1002.h"
#include "1003.h"
#include "1004.h"
#include "1201.h"
#include "1301.h"
#include "1302.h"
#include "1303.h"

OgreBites::SamplePlugin* gPlugin;

extern "C" _OgreSampleExport void dllStartPlugin()
{
 gPlugin = OGRE_NEW OgreBites::SamplePlugin("NxOgreTutorials Sample");
 gPlugin->addSample(new NxOgreTest);
 gPlugin->addSample(new NxOgre101);
 gPlugin->addSample(new NxOgre102);
 gPlugin->addSample(new NxOgre103);
 gPlugin->addSample(new NxOgre104);
 gPlugin->addSample(new NxOgre105);
 gPlugin->addSample(new NxOgre106);
 gPlugin->addSample(new NxOgre107);
 gPlugin->addSample(new NxOgre108);
 gPlugin->addSample(new NxOgre109);
 gPlugin->addSample(new NxOgre110);
 gPlugin->addSample(new NxOgre111);
 gPlugin->addSample(new NxOgre113);
 gPlugin->addSample(new NxOgre114);
 gPlugin->addSample(new NxOgre117);
 gPlugin->addSample(new NxOgre303);
 gPlugin->addSample(new NxOgre304);
 gPlugin->addSample(new NxOgre304a);
 gPlugin->addSample(new NxOgre305);
 gPlugin->addSample(new NxOgre701);
 gPlugin->addSample(new NxOgre702);
 gPlugin->addSample(new NxOgre1001);
 gPlugin->addSample(new NxOgre1002);
 gPlugin->addSample(new NxOgre1003);
 gPlugin->addSample(new NxOgre1004);
 gPlugin->addSample(new NxOgre1201);
 gPlugin->addSample(new NxOgre1301);
 gPlugin->addSample(new NxOgre1302);
 gPlugin->addSample(new NxOgre1303);
 
 Ogre::Root::getSingleton().installPlugin(gPlugin);
}

extern "C" _OgreSampleExport void dllStopPlugin()
{
 Root::getSingleton().uninstallPlugin(gPlugin); 
 SampleSet tutorials = gPlugin->getSamples();
 OGRE_DELETE gPlugin;
 for (SampleSet::iterator it=tutorials.begin();it != tutorials.end();it++)
  delete (*it);
}
