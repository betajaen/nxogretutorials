@echo off

:: ----------------------------------------------------
IF "%1" == "maketutorials" GOTO MakeTutorials 
EXIT

:: ----------------------------------------------------
:: MakeTutorials
:: ----------------------------------------------------
:MakeTutorials
IF "%2" == "debug" GOTO MakeTutorialsDebug
IF "%2" == "final" GOTO MakeTutorialsFinal
GOTO BuildBotExit

:: ----------------------------------------------------
:: MakeTutorialsDebug
:: ----------------------------------------------------
:MakeTutorialsDebug
echo [BuildBot] Copying Tutorials Debug
xcopy "%2\*.dll" "%OGRE_HOME%/bin/debug" /Q /R /Y
xcopy "%NXOGRE_DIR%\sdk\NxOgreDebug.dll" "%OGRE_HOME%/bin/debug" /F /R /Y
IF NOT EXIST "%OGRE_HOME%/bin/debug/NxCharacter.dll" xcopy "%PHYSX_DIR%\bin\win32\NxCharacter.dll" "%OGRE_HOME%/bin/debug" /F /R /Y
IF NOT EXIST "%OGRE_HOME%/bin/debug/PhysXCooking.dll" xcopy "%PHYSX_DIR%\bin\win32\PhysXCooking.dll" "%OGRE_HOME%/bin/debug" /F /R /Y
IF NOT EXIST "%OGRE_HOME%/bin/debug/PhysXCore.dll" xcopy "%PHYSX_DIR%\bin\win32\PhysXCore.dll" "%OGRE_HOME%/bin/debug" /F /R /Y
IF NOT EXIST "%OGRE_HOME%/bin/debug/PhysXDevice.dll" xcopy "%PHYSX_DIR%\bin\win32\PhysXDevice.dll" "%OGRE_HOME%/bin/debug" /F /R /Y
IF NOT EXIST "%OGRE_HOME%/bin/debug/PhysXLoader.dll" xcopy "%PHYSX_DIR%\bin\win32\PhysXLoader.dll" "%OGRE_HOME%/bin/debug" /F /R /Y
IF NOT EXIST "%OGRE_HOME%/bin/debug/PhysXLoader.dll" xcopy "%PHYSX_DIR%\bin\win32\PhysXLoader.dll" "%OGRE_HOME%/bin/debug" /F /R /Y
IF NOT EXIST "%OGRE_HOME%/media/packs/nxogre.zip" xcopy "..\..\media\nxogre.zip" "%OGRE_HOME%/media/packs/" /F /R /Y

findstr /i /m "SamplePlugin=NxOgreTutorialsDebug.dll" "%OGRE_HOME%\bin\debug\samples_d.cfg" >NUL 2>&1
IF ERRORLEVEL 1 GOTO D1
GOTO D2
:D1
ECHO. >> "%OGRE_HOME%\bin\debug\samples_d.cfg"
ECHO SamplePlugin=NxOgreTutorialsDebug.dll >> "%OGRE_HOME%\bin\debug\samples_d.cfg"
:D2

findstr /i /m "Zip=../../media/packs/NxOgre.zip" "%OGRE_HOME%\bin\debug\resources_d.cfg" >NUL 2>&1
IF ERRORLEVEL 1 GOTO D3
GOTO D4
:D3
ECHO. >> "%OGRE_HOME%\bin\debug\resources_d.cfg"
ECHO [Popular] >> "%OGRE_HOME%\bin\debug\resources_d.cfg"
ECHO Zip=../../media/packs/NxOgre.zip >> "%OGRE_HOME%\bin\debug\resources_d.cfg"
:D4


GOTO BuildBotExit
:: ----------------------------------------------------


:: ----------------------------------------------------
:: MakeTutorialsFinal
:: ----------------------------------------------------
:MakeTutorialsFinal
echo [BuildBot] Copying Tutorials Release
xcopy "%2\*.dll" "%OGRE_HOME%/bin/release" /Q /R /Y
xcopy "%NXOGRE_DIR%\sdk\NxOgre.dll" "%OGRE_HOME%/bin/release" /F /R /Y
IF NOT EXIST "%OGRE_HOME%/bin/release/NxCharacter.dll" xcopy "%PHYSX_DIR%\bin\win32\NxCharacter.dll" "%OGRE_HOME%/bin/release" /F /R /Y
IF NOT EXIST "%OGRE_HOME%/bin/release/PhysXCooking.dll" xcopy "%PHYSX_DIR%\bin\win32\PhysXCooking.dll" "%OGRE_HOME%/bin/release" /F /R /Y
IF NOT EXIST "%OGRE_HOME%/bin/release/PhysXCore.dll" xcopy "%PHYSX_DIR%\bin\win32\PhysXCore.dll" "%OGRE_HOME%/bin/release" /F /R /Y
IF NOT EXIST "%OGRE_HOME%/bin/release/PhysXDevice.dll" xcopy "%PHYSX_DIR%\bin\win32\PhysXDevice.dll" "%OGRE_HOME%/bin/release" /F /R /Y
IF NOT EXIST "%OGRE_HOME%/bin/release/PhysXLoader.dll" xcopy "%PHYSX_DIR%\bin\win32\PhysXLoader.dll" "%OGRE_HOME%/bin/release" /F /R /Y
IF NOT EXIST "%OGRE_HOME%/bin/release/PhysXLoader.dll" xcopy "%PHYSX_DIR%\bin\win32\PhysXLoader.dll" "%OGRE_HOME%/bin/release" /F /R /Y
IF NOT EXIST "%OGRE_HOME%/media/packs/nxogre.zip" xcopy "..\..\media\nxogre.zip" "%OGRE_HOME%/media/packs/" /F /R /Y

findstr /i /m "SamplePlugin=NxOgreTutorials.dll" "%OGRE_HOME%\bin\release\samples.cfg" >NUL 2>&1
IF ERRORLEVEL 1 GOTO R1
GOTO R2
:R1
ECHO. >> "%OGRE_HOME%\bin\release\samples.cfg"
ECHO SamplePlugin=NxOgreTutorials.dll >> "%OGRE_HOME%\bin\release\samples.cfg"
:R2

findstr /i /m "Zip=../../media/packs/NxOgre.zip" "%OGRE_HOME%\bin\release\resources.cfg" >NUL 2>&1
IF ERRORLEVEL 1 GOTO R3
GOTO R4
:R3
ECHO. >> "%OGRE_HOME%\bin\release\resources.cfg"
ECHO [Popular] >> "%OGRE_HOME%\bin\release\resources.cfg"
ECHO Zip=../../media/packs/NxOgre.zip >> "%OGRE_HOME%\bin\release\resources.cfg"
:R4

GOTO BuildBotExit
:: ----------------------------------------------------


:BuildBotExit