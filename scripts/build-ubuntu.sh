#!/bin/bash

cmake -S . -B build
cmake --build build

mkdir -p CCSakura-linux/{bin,lib,assets}

echo "Copying binary"
cp -v build/bin/CCSakura CCSakura-linux/bin/

echo "Copying assets"
cp -v -r assets/* CCSakura-linux/assets/

echo "Copying libraries"
ldd build/bin/CCSakura | grep "=> /" | awk '{print $3}' | xargs -I '{}' cp -v '{}' CCSakura-linux/lib/

echo "Making build script"
cat > CCSakura-linux/CCSakura.sh << 'EOF'
#!/bin/bash
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
export LD_LIBRARY_PATH="$SCRIPT_DIR/lib:$LD_LIBRARY_PATH"
"$SCRIPT_DIR/bin/CCSakura" "$@"
EOF

chmod +x CCSakura-linux/CCSakura.sh

echo "Archiving package"
tar -czvf CCSakura-linux.tar.gz CCSakura-linux/

echo "Moving to artifacts for distribution"
mkdir -p artifacts
mv -v CCSakura-linux.tar.gz artifacts/CCSakura-linux.tar.gz
