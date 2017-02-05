We use the Qt-Installer Framework for the installer.

# Prerequisites:

Download the [prebuilt framework](https://download.qt.io/official_releases/qt-installer-framework/)
The os name in the framework specifies for which os the installer is generated

Naming convention: `erasim-<version>-<platform>` (e.g. erasim-0.9.0-linux)

Note: for zipping you can use `archivegen <file to zip> <output name>` from the framework

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
* run `<path_to_qt_mingw>\bin\windeployqt.exe --relase era-sim.exe`
* Copy additional libraries from `<qt_mingw>\bin` into temporary folder: libgcc_s_dw2-1.dll, libstdc++-6.dll, libwinpthread-1.dll
* Copy qml Dependencies from `<qt_mingw>\qml\` into temporary folder: Qt, QtGraphicalEffects, QtQml, QtQuick, QtQuick.2
* Copy isa and themes folder into temporary folder
* Zip the contents of the temporary folder into `packages\erasim.base\data\base.7z`
* goto `installer\`
* run `<path_to_framework>\bin\binarycreator.exe -c config\config.xml -p packages <name of installer>`
* The installer is created inside this folder
* Copy it into the respective release folder

# macOS release
* Build a new clean version of era-sim (preferable with '-D CMAKE_BUILD_TYPE=Release'
  that makes the final executable much smaller)
* Create a temporary folder
* Copy era-sim executable into temp folder
* Copy frameworks from `<path-to-qt>/clang_64/lib/` to temp folder:
  * You can check the frameworks linked by the binary created above by running
    `otool -L <path-to-binary>/era-sim`
* Copy QML dependencies from `<path-to-qt>/clang_64/qml/` to temp folder:
  Qt, QtGraphicalEffects, QtQml, QtQuick, QtQuick.2
* Copy isa- and themes-folder from repository to temp folder
* Add icons
  * Create a folder named `icon.iconset` (not inside the temp folder)
  * Move png-files of the required icon sizes to `icon.iconset`. Required sizes
    listed in Apple's Human Interface Guidelines (https://developer.apple.com/library/content/documentation/UserExperience/Conceptual/OSXHIGuidelines/Designing.html)
  * Change to parent directory
  * Use iconutil to create .icns file: `iconutil -c icns icon.iconset`
  * Create folder `<tmp>/Contents` and `<tmp>/Contents/Resources`
  * Move `icon.icns` to `<tmp>/Contents/Resources`
* Move `<repository>/installer/config/info.plist` to `<tmp>/Contents/`
* Change directory to temp folder
* Zip folder to 7z-archive via `<path_to_qt_installer_framwork>/bin/archivegen base.7z ./`
* Move archive to `<repository>/installer/packages/erasim.base/data/base.7z`
* Change directory to `<repository>/installer`
* Use binarycreator-tool of Qt-installer-framework to create installer:
  `<path_to_qt_installer_framwork>/bin/binarycreator -c config/config_mac.xml -p packages erasim`
* The installer is created inside the current folder
* Copy it into the respective release folder
