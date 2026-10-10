# QmlForge Packaging Script for Windows
$ErrorActionPreference = "Stop"

$projectDir = $PSScriptRoot
$buildDir = "$projectDir\build"
$distDir = "$projectDir\dist"
$appDistDir = "$distDir\QmlForge"
$zipPath = "$distDir\QmlForge-v1.0.0-Windows-x64.zip"

Write-Host "==> [1/5] Cleaning previous build and dist artifacts..." -ForegroundColor Cyan
if (Test-Path $distDir) { Remove-Item -Recurse -Force $distDir }
if (Test-Path $buildDir) { Remove-Item -Recurse -Force $buildDir }

New-Item -ItemType Directory -Path $buildDir | Out-Null
New-Item -ItemType Directory -Path $appDistDir | Out-Null

$qtDir = "C:\Qt\6.8.3\mingw_64"
$env:PATH = "$qtDir\bin;C:\Qt\Tools\mingw1310_64\bin;C:\Qt\Tools\Ninja;$env:PATH"

Write-Host "==> [2/5] Configuring CMake Release build..." -ForegroundColor Cyan
cmake -B $buildDir -G "Ninja" -DCMAKE_PREFIX_PATH="$qtDir" -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER="C:/Qt/Tools/mingw1310_64/bin/g++.exe" -DCMAKE_C_COMPILER="C:/Qt/Tools/mingw1310_64/bin/gcc.exe"

Write-Host "==> [3/5] Building QmlForge in Release mode..." -ForegroundColor Cyan
cmake --build $buildDir --config Release

$exePath = "$buildDir\QmlForge.exe"
if (-not (Test-Path $exePath)) {
    throw "Build failed: QmlForge.exe not found at $exePath"
}

Copy-Item $exePath -Destination $appDistDir

Write-Host "==> [4/5] Running windeployqt with QML runtime modules..." -ForegroundColor Cyan
$windeployqt = "$qtDir\bin\windeployqt.exe"
& $windeployqt --release --qmldir "$projectDir" "$appDistDir\QmlForge.exe"

Write-Host "==> [5/5] Creating zip package..." -ForegroundColor Cyan
Compress-Archive -Path "$appDistDir\*" -DestinationPath $zipPath -Force

Write-Host "Successfully packaged QmlForge!" -ForegroundColor Green
Write-Host "Zip Archive: $zipPath" -ForegroundColor Green
