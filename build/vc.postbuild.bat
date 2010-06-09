xcopy "%1\%2\*.dll" "%OGRE_HOME%\bin\%3\" /Q /R /Y

IF %3==debug GOTO CopyDebug

:CopyRelease
xcopy "%NXOGRE_DIR%\SDK\NxOgre.dll" "%OGRE_HOME%\bin\%3\" /Q /R /Y
findstr /i /m "SamplePlugin=%1" "%OGRE_HOME%\bin\%3\samples.cfg" >NUL 2>&1
IF ERRORLEVEL 1 GOTO WriteReleaseSample
GOTO Finished

:WriteReleaseSample
ECHO. >> "%OGRE_HOME%\bin\%3\samples.cfg"
ECHO SamplePlugin=%1 >> "%OGRE_HOME%\bin\%3\samples.cfg"
GOTO Finished



:CopyDebug
xcopy "%NXOGRE_DIR%\SDK\NxOgreDebug.dll" "%OGRE_HOME%\bin\%3\" /Q /R /Y
findstr /i /m "SamplePlugin=%1Debug" "%OGRE_HOME%\bin\%3\samples_d.cfg" >NUL 2>&1
IF ERRORLEVEL 1 GOTO WriteDebugSample
GOTO Finished

:WriteDebugSample
ECHO. >> "%OGRE_HOME%\bin\%3\samples_d.cfg"
ECHO SamplePlugin=%1Debug >> "%OGRE_HOME%\bin\%3\samples_d.cfg"
GOTO Finished


:Finished
