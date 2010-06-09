NxOgre Tutorials
================

Copyright and Written by Robin Southern, 2010. See LICENCE files for further information.


These are a collection of tutorials based of the PhysX tutorials you would find in the Windows PhysX SDK. In most cases the PhysX tutorial has been rewritten in NxOgre and Ogre code but with the spirit of the tutorial still remaining.

Much of the context and tutorial text will still apply, but will be named slightly difference; `NxActor` --> `Actor` or `Body`. 

For more information and help visit the Tutorials thread at the NxOgre forum.

Installing the tutorials
------------------------

Before compiling make sure you have the NXOGRE_DIR and OGRE_HOME environmental variables correctly set.

When compiling the tutorials, they will auto-install themselves into the debug or release version samples folder.


Installing NxOgre Media
-----------------------

Simply copy the zip file to the "packs" folder inside the Ogre media folder; i.e. `C:\dev\Ogre\media\packs`


Then in the Ogre bin folder either in debug or release, you need to add the following line to the `resources(_d).cfg` file. Just copy and paste it underneath `Zip=../../media/packs/skybox.zip`

    Zip=../../media/packs/NxOgre.zip

That's it!

Running the tutorials
---------------------

Simply run the `SamplesBrowser.exe` (or `SamplesBrowser_d.exe`), if everything is correctly installed. You shall see them under the Physics section.