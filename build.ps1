$env:PATH = "C:\Qt\6.8.3\mingw_64\bin;C:\Qt\Tools\mingw1310_64\bin;C:\Qt\Tools\Ninja;" + $env:PATH
if (Test-Path build) { Remove-Item -Recurse -Force build }
cmake -B build -G "Ninja" -DCMAKE_PREFIX_PATH="C:\Qt\6.8.3\mingw_64" -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER="C:/Qt/Tools/mingw1310_64/bin/g++.exe" -DCMAKE_C_COMPILER="C:/Qt/Tools/mingw1310_64/bin/gcc.exe"
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
cmake --build build --config Release
