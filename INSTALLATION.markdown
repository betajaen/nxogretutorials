Installing the tutorials
========================

Before compiling make sure you have the NXOGRE_DIR and OGRE_HOME environmental variables correctly set.

When compiling the tutorials, they will auto-install themselves into the debug or release version samples folder.


Installing NxOgre Media
=======================

Simply copy the zip file to the "packs" folder inside the Ogre media folder; i.e. `C:\dev\Ogre\media\packs`


Then in the Ogre bin folder either in debug or release, you need to add the following line to the `resources(_d).cfg` file. Just copy and paste it underneath `Zip=../../media/packs/skybox.zip`

    Zip=../../media/packs/NxOgre.zip

That's it!

Running the tutorials
=====================

Simply run the `SamplesBrowser.exe` (or `SamplesBrowser_d.exe`), if everything is correctly installed. You shall see them under the Physics section.