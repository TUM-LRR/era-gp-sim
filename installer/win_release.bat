:: Release script for windows
:: Parameters
::   <path to mingw binaries of qt> (Example: C:\Qt\Qt5.8.0\5.8\mingw53_32\bin)
::   <path to installer framework binaries> (Example: C:\Qt\QtIFW2.0.3\bin)
::   <name of the output file>


:: NOTE: This script does not compile the simulator. It only takes the built executable for installing
SET erasim_exe=era-sim.exe
SET root=%~dp0..
:: assume build in build

mkdir %root%\windeploy
xcopy %root%\build\bin\%erasim_exe% %root%\windeploy /H /E /Y
cd %root%\windeploy\

%1\windeployqt.exe %erasim_exe%

::Copy additional libs
xcopy %1\libgcc_s_dw2-1.dll %root%\windeploy /H /E /Y
xcopy %1\libstdc++-6.dll %root%\windeploy /H /E /Y
xcopy %1\libwinpthread-1.dll %root%\windeploy /H /E /Y

ECHO Zipping isa-Folder, config things
mkdir %root%\windeploy\.erasim
xcopy %root%\isa %root%\windeploy\.erasim\isa\ /H /E /Y
xcopy %root%\themes %root%\windeploy\.erasim\themes\ /H /E /Y
%2\archivegen.exe %root%\installer\packages\erasim.config\data\config.7z %root%\windeploy\.erasim

rmdir /S /Q %root%\windeploy\.erasim

ECHO Zipping dlls. This takes a long time without any output
%2\archivegen.exe %root%\installer\packages\erasim.base\data\base.7z %root%\windeploy\*

mkdir %root%\release
%2\binarycreator.exe -c %root%\installer\config\config.xml -p %root%\installer\packages\ %root%\release\%3

cd %~dp0
::cleanup
rmdir /S /Q %root%\windeploy
del %root%\installer\packages\erasim.base\data\base.7z
del %root%\installer\packages\erasim.config\data\config.7z