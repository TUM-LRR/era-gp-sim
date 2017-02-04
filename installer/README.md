We use the Qt-Installer Framework for the installer.

# Prerequisites:

Download the [prebuilt framework](https://download.qt.io/official_releases/qt-installer-framework/)
The os name in the framework specifies for which os the installer is generated

Naming convention: erasim-<version>-<platform> (e.g. erasim-0.9.0-linux)

Note: for zipping you can use archivegen <file to zip> <output name> from the framework

# Linux release
* Build a new clean version of era-sim (prefereable with `-D CMAKE_BUILD_TYPE=Release`
  that makes the final executable much smaller)
* Use the `scripts/release-linux.sh` script to create the base.7z
  (e.g. cd into project root and `release-linux.sh -e build/bin/era-sim -r ./ -q /usr/lib/qt -7`)
* Move base.7z to `installer/packages/erasim.base/data/base.7z`
* cd into `installer/`
* run `<path_to_framework>/bin/binarycreator -c config/config.xml -p packages <name of installer>`
* The installer is created inside this folder
* Copy it into the respective release folder

# Windows release (using mingw 32bit)
* Build a new clean version of era-sim
* Create temporary folder, copy era-sim executable into temp folder
* goto temporary folder
* run <path_to_qt_mingw>\bin\windeployqt.exe --relase era-sim.exe
* Copy additional libraries from <qt_mingw>\bin into temporary folder: libgcc_s_dw2-1.dll, libstdc++-6.dll, libwinpthread-1.dll
* Copy qml Dependencies from <qt_mingw>\qml\ into temporary folder: Qt, QtGraphicalEffects, QtQml, QtQuick, QtQuick.2
* Copy isa and themes folder into temporary folder
* Zip the contents of the temporary folder into packages\erasim.base\data\base.7z
* goto installer\
* run <path_to_framework>\bin\binarycreator.exe -c config\config.xml -p packages <name of installer>
* The installer is created inside this folder
* Copy it into the respective release folder

