REM Set the path to your source code directory
set sourceDir=./

REM Set the path to the directory where you want to generate the Visual Studio project file
set buildDir=./vs2022

REM Check if buildDir exists, if not, create it
if not exist %buildDir% (
    mkdir %buildDir%
)

REM Run CMake to generate the Visual Studio project file
cmake -S %sourceDir% -B %buildDir% -G "Visual Studio 17 2022"

REM Check if the project file was generated successfully
if exist %buildDir%\Software-Renderer.sln (
    echo Visual Studio project file generated successfully.
) else (
    echo Failed to generate Visual Studio project file.
)
