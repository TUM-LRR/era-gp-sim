#!/bin/sh

while [[ $# -gt 0 ]]; do
  key="$1"

  case $key in
    (--help)
      echo "This is the help"
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

#TODO
