/*! page. NxOgre101
 
p. These tutorials use the Ogre3D Rendering System and Critter

h2. Migrating from PhysX

This tutorial is equivalent to "Lesson 101 - Primary Shapes" provided by the PhysX SDK.

h2. Introduction

101 covers the very basics of NxOgre and PhysX. In this tutorial we are going to setup NxOgre and a Scene. In that scene a box will be created, and a flat infinite plane for that cube to sit on. Then we can apply forces to that box via the keyboard.

h2. Part 1 - Initialising NxOgre and PhysX.

In any application using NxOgre you will need to initialise PhysX and NxOgre, this is both achieved by creating the NxOgre World class. This is the master class, and a singleton - so there can be only one at a time.

World* mWorld = World::destroyWorld();

Later on, we can destroy World (and the contents within):

World::destroyWorld();

h2. Part 2 - Creating the Scene.

The Scene is the next thing to be created, they are owned by the World and there can be up to 32 of them at a time, anything in one Scene cannot interact with anything in others.

We create a Scene via a SceneDescription, then pass it to World::createScene

bc. NxOgre::SceneDescription scene_description;
    scene_description.mGravity = NxOgre::SceneDescription::MEAN_EARTH_GRAVITY;
    scene_description.mUseHardware = false;
    Scene* mScene = mWorld->createScene(scene_description);

You'll find Descriptions throughout NxOgre, they can be used one and discarded or used many times. Descriptions automatically take on the default PhysX settings, these are given in the comment header for each member variable in a description, these are also found in the PhysX documentation.

h2. Part 3 - Adding materials.

p. A material defines the collision and surface properties of a object within a scene; how they bounce, slide or roll off other objects. When creating a scene, a material is automatically created for you. We call this the default material, in other words - any object given without a material will take on the default one.

p. Each material is given an index (a numerical identifier), and the default material uses 0.

bc. Material* defaultMaterial = mScene->getMaterial(0);
    defaultMaterial->setRestitution(0.5f);
    defaultMaterial->setDynamicFriction(0.5f);
    defaultMaterial->setStaticFriction(0.5f);

p. As you can see, the bounce (restitution) has been set to 0.5 of a maximum of 1.0, and both types of frictions have been given an average amount. This pretty much the most average material you can get.


h2. Part 4 - Creating Actors in the Scene.

p. First of all, we need a floor, or a Plane. Planes are used to stop things falling into enternity or to limit parts of your scene. All Planes are infinite in size, but can be in any direction or distance from the Scene center.

p. Planes can be only attached to static Actors; in NxOgre we call these "SceneGeometries". SceneGeometries can never be moved, adjusted or even deleted within a Scene. The reason being that PhysX assumes them to be static, so it can make assumptions about them. Imagine if you had a stack of books on a table, and you removed the table. PhysX assumes the table will never be removed, so it assumes that the books will never fall. So when you remove the table, the books won't fall, but stay as they are. Eventually the hero walks into the kitchen, and bumps against the books - which in this case they will fall - but not before your hero realises what a strange world he lives in!

p. Actors and SceneGeometries belong to Scenes, so we create them using the Scene. Actors and SceneGeometries both use RigidBodyDescriptions to describe what they are, but I'm starting of with the plane and there is nothing special about that. So I can omit the description.

bc. mScene->createSceneGeometry(PlaneGeometryDescription());

p. That's the plane created; the only argument is the ShapeDescription to describe the plane. Which uses the default up direction. The other arguments use the default SceneCenter and the default RigidBodyDescription which is perfect for the planes needs.

h2. Part 5 - Visualisation

p. Next we need our Actor; we will be attaching a box to it to give it a physical presence. All Actors (and SceneGeometries) must have at least one type of shape attached to them. The next step is to decide how we want it to appear on the screen. You may of noticed I haven't given any visual identification to the plane on how it should be rendered on the screen. There is a good reason for this.

p. Anything you create with NxOgre will be invisible. Your thinking what's the point of NxOgre then? Well we have a clever thing call RenderSystems which NxOgre interfaces with to render with. You see NxOgre doesn't really care about rendering things, or who it renders too. It isn't partial to any 3D or 2D library, that way it can work with Ogre, OpenGL or not even bother with one at all. 

p. In this case, we are using Ogre. So we will use "Critter" the RenderSystem written to work with Ogre. If we want things rendered in Ogre, we create them through Critter. In general terms; any Actor rendered is given the term a "Body". Bodies are Actors with an extra bit of visual information in them, and they thrown into any of the NxOgre or Critter functions that work with them because in the long run NxOgre sees them as Actors and Critter sees them as Bodies.

p. So let's initialize Critter.

bc. mRenderSystem = new Critter::RenderSystem(mScene, mSceneMgr);

p. mScene is our Scene we have created, and mSceneMgr is the Ogre SceneManager we want to use. We can delete the 
   RenderSystem later on via @delete mRenderSystem@. Remember it owns the Bodies, so they will be destroyed too.

h2. Part 6 - Bodies

p. Now, let's create our visualised Actor - a Body. The rendersystem is responsible for visualisation so we create
   it through that way, later on when we don't need the Body - the rendersystem is responsible for deleting it.

p. Let's start off with a more simple one, and use one of the friendly createBody functions.

bc.. Critter::BodyDescription bodyDescription;
     bodyDescription.mMass = 40.0f;
     Body* mBody = mRenderSystem->createBody(NxOgre::BoxDescription(1,1,1), Vec3(0,1,0), "cube.1m.mesh", bodyDescription);

p. In this case, our body has been created. It has been given a physical shape of a 1m^3^ box, and placed
   1 metre of the ground. The @cube.1m.mesh@ is shorthand for; create an Entity using "cube.1m.mesh" then
   attach it to a new SceneNode and give that to the Body. Finally we pass on the "BodyDescription" to give
   it some mass of 40.0 kilograms.

p. There are four createBody functions, and this type used it one of two of the helpers. This type of helper
   automatically creates the Entity and SceneNode setup for us, but for more complicated Bodies you may want
   to create the SceneNode yourself (or use a pre-existing one), in that case you would use an alternative
   createBody function and manually give the bodyDescription the node pointer (and destruction policy) yourself.

p. You'll find quite a few helper functions like this in NxOgre, as your new to it now. You should use them.

h2. Part 7 - Making it go!

p. We've done nearly all of it now. Created the World, Scene, Ground Plane, RenderSystem and our Body. We need to
   make it go - make the body fall on to the ground.

p. That is achieved via the TimeController, which you may of guessed by its name controls time.

p. TimeController is a singleton, and created when World is created -- You shouldn't need to destroy it.

bc. TimeController* mTimeController = NxOgre::TimeController::getSingleton();

p. It is up to you to "inject" time into NxOgre, usually this is done every frame of rendering, and the time
   passed, is the time since last frame. We are using Ogre, so it is fairly easy to bind and do.

bc.. bool frameRenderingQueued(const FrameEvent& evt)
     {
      mTimeController->advance(evt.timeSinceLastFrame);
      return SdkSample::frameRenderingQueued(evt);
     }

p. This injection of time will trickle down through the main classes and other classes you haven't heard of yet.
   Bodies are one of these classes, and will automatically update when needed. 

h2. Part 8 - Adding forces.

p. Finally, to add forces (measured in newtons), we use the addForce function which is inherited from Actor.

bc. body->addForce(Vec3(0,100 * lastTimeStep,0));

*/
