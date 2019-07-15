#!/bin/bash
project_dir=$(pwd)

# Complain when not in Travis environment
if [ -z ${TRAVIS_COMMIT+x} ]; then
    echo "This script is intended to be used only in Travis CI environment."
    exit 1
fi

brew update > /dev/null
brew install qt

QTDIR="/usr/local/opt/qt"
PATH="$QTDIR/bin:$PATH"
LDFLAGS=-L$QTDIR/lib
CPPFLAGS=-I$QTDIR/include

# Build your app
cd ${project_dir}
cd 3rdparty/scintilla/qt/ScintillaEdit
python WidgetGen.py
cd ${project_dir}
mkdir build
cd build
qmake -v
qmake CONFIG+=qtquickcompiler CONFIG+=release ../cjlv.pro
if [ $? -ne 0 ]; then 
    exit 1; 
fi

make -j2
if [ $? -ne 0 ]; then 
    exit 1; 
fi

mkdir -p distrib/cjlv
cd distrib/cjlv
mv ../../*.app ./
cp "${project_dir}/README.md" "README.md"
echo "${version}" > version
echo "${TRAVIS_COMMIT}" >> version

ln -s /Applications ./Applications

cd ..
hdiutil create -srcfolder ./cjlv -format UDBZ ./cjlv.dmg
mv cjlv.dmg cjlv-${version}-x64.dmg
cd ..

exit 0
