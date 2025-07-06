#!/bin/bash

cmake -S . -B build -DCMAKE_OSX_DEPLOYMENT_TARGET=10.13
cmake --build build
cmake --install .
ls

mkdir -p artifacts

echo "Copying libraries to frameworks"
cp -R -v build/lib/. build/bin/CCSakura.app/Contents/Frameworks

echo "Copying assets to resources"
mkdir -p build/bin/CCSakura.app/Contents/Resources/assets
cp -R -v assets/. build/bin/CCSakura.app/Contents/Resources/assets

echo "Moving to artifacts for distribution"
mv -v build/bin/CCSakura.app artifacts/CCSakura.app

echo "Install name tool"
install_name_tool -add_rpath "@executable_path/../Frameworks" "artifacts/CCSakura.app/Contents/MacOS/CCSakura"

echo "Archiving MacOS"
zip -r ./artifacts/CCSakura.zip ./artifacts/CCSakura.app
mv -v ./artifacts/CCSakura.app CCSakura.app
