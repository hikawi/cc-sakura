Write-Host "Building CMake files"
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
Write-Host "Building binary files"
cmake --build build --config Release

mkdir -p artifacts
mkdir -p CCSakura-Win

Write-Host "Viewing the directories of build\bin\"
Get-ChildItem -Recurse ".\build\bin\"

Write-Host "Viewing the directories of build\lib\"
Get-ChildItem -Recurse ".\build\lib\"

Write-Host "Copying assets"
Copy-Item -Path ".\build\bin\Release\*" -Destination ".\CCSakura-Win\" -Recurse -Force -Verbose
Copy-Item -Path ".\assets" -Destination ".\CCSakura-Win\assets" -Recurse -Force -Verbose
