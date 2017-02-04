#!/bin/sh

while [[ $# -gt 0 ]]; do
  key="$1"

  case $key in
    (--help)
      echo "This script collects all needed files for a release on linux"
      echo "Options:"
      echo " -e, --executable | The era-sim executable (e.g. build/bin/era-sim)"
      echo " -i, --isa        | The isa directory"
      echo " -t, --themes     | The themes directory"
      echo " -q, --qt         | The qt installation path (e.g. /usr/lib/qt)"
      echo " -d, --directory  | The directory, where all files should be put in."
      echo "                    Defaults to ./era-sim."
      exit 0
      ;;
    (-e|--executable)
      EXEC="$2"
      shift
      ;;
    (-i|--isa)
      ISA="$2"
      shift
      ;;
    (-t|--themes)
      THEMES="$2"
      shift
      ;;
    (-d|--directory)
      DIR="$2"
      shift
      ;;
    (-q|--qt)
      QT="$2"
      shift
      ;;
    (*)
      echo "Unknown option $key"
      exit 1
      ;;
  esac
  shift
done

if [[ ! -x $EXEC ]]; then
  echo "Executable needed. See --help"
  exit 1
fi

if [[ ! -d $ISA ]]; then
  echo "Isa folder needed. See --help"
  exit 1
fi

if [[ ! -d $THEMES ]]; then
  echo "Themes folder needed. See --help"
  exit 1
fi

if [[ ! -d $QT ]]; then
  echo "Qt installation dir needed. See --help"
  exit 1
fi

if [[ -z $DIR ]]; then
  DIR="./era-sim"
  mkdir -v $DIR
fi

echo "Copying executable..."
cp $EXEC $DIR

echo "Copying isa dir..."
cp -r $ISA $DIR

echo "Copying themes dir..."
cp -r $THEMES $DIR

echo "Populating libs..."
mkdir $DIR/libs
cp $(ldd $EXEC | cut -d" " -f3) $DIR/libs

echo "Searching for libqxcb.so..."
mkdir $DIR/platforms
cp $(find $QT -name "libqxcb.so") $DIR/platforms

echo "Collecting libraries for libqxcb.so..."
cp $(ldd $DIR/platforms/libqxcb.so | cut -d" " -f3) $DIR/libs

echo "Updating file permissions for libs..."
chmod 755 $DIR/libs/*

echo "Removing OpenGL lib..."
rm $(find $DIR/libs/ -name "libGL.so*")

echo "Copying qml runtime..."
mkdir $DIR/qml
cp -r $QT/qml/QtGraphicalEffects $DIR/qml/
cp -r $QT/qml/QtQml $DIR/qml/
cp -r $QT/qml/QtQuick $DIR/qml/
cp -r $QT/qml/QtQuick.2 $DIR/qml/

echo "Copying plugins..."
mkdir $DIR/plugins
cp -r $QT/plugins/xcbglintegrations $DIR/plugins/
cp -r $QT/plugins/imageformats $DIR/plugins/
cp -r $QT/plugins/iconengines $DIR/plugins/

echo "Creating era-sim.sh..."
touch $DIR/era-sim.sh
cat <<EOM > $DIR/era-sim.sh
#!/bin/sh

export LD_LIBRARY_PATH="./libs:\$LD_LIBRARY_PATH"

./era-sim
EOM
chmod 755 $DIR/era-sim.sh

echo "Creating qt.conf..."
touch $DIR/qt.conf
cat <<EOM > $DIR/qt.conf
[Paths]
Plugins = plugins
EOM

echo "Creating era-sim.tar.gz..."
tar czf era-sim.tar.gz $DIR

echo -e "\nFinished"
