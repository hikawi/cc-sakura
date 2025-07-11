Write-Host "Building CMake files"
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -G Ninja -DCMAKE_CXX_FLAGS="-Wa,-mbig-obj" -DCMAKE_TOOLCHAIN_FILE="$Env:GITHUB_WORKSPACE\vcpkg\scripts\buildsystems\vcpkg.cmake"

if($? -ne 0)
{
    Write-Host "CMake build failed. Aborting..."
    exit(1)
}

Write-Host "Building binary files"
cmake --build build

if($? -ne 0)
{
    Write-Host "Binary build failed. Aborting..."
    exit(1)
}

mkdir -p artifacts
mkdir -p CCSakura-Win

Write-Host "Viewing the directories of build\bin\"
Get-ChildItem -Recurse ".\build\bin\"

Write-Host "Viewing the directories of build\lib\"
Get-ChildItem -Recurse ".\build\lib\"

Write-Host "Copying assets"
Copy-Item -Path ".\build\bin\*" -Destination ".\CCSakura-Win\" -Recurse -Force -Verbose
Copy-Item -Path ".\assets" -Destination ".\CCSakura-Win\assets" -Recurse -Force -Verbose

Write-Host "Copying libraries"
Copy-Item -Path ".\build\lib\*" -Destination ".\CCSakura-Win\" -Recurse -Force -Verbose

Write-Host "Archiving"
Compress-Archive -Path ".\CCSakura-Win\*" -DestinationPath ".\artifacts\CCSakura-Win.zip"
