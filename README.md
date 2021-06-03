

# Build

## Using a Visual Studio 2019

- Install [Microsoft Visual Studio 2019 Community](https://visualstudio.microsoft.com/downloads/#visual-studio-community-2019)
- Open `src/NWNX4.sln` with Visual Studio
- Press F6 or go to _Build -> Build solution_


# Work in progress

[ ] nwnx_gui should be ported to use latest Qt version
[ ] xp_ruby: I could not find any up-to-date prebuilt Ruby C library to link against
[ ] xp_chat does not work (error triggered when a player enters the server). This plugin is probably useless since you can set a custom OnChat script in the module properties.

# Build (2021/06/02)

This set of instructions should help build the source code for the new decade.

## Requirements

- Windows
- CMake
- Git
- MSBuild
- PowerShell

## Instructions

The following dependencies are required for NWNx4:

- Detours
- wxWidgets

The most recent version of these dependencies are large. They are so large, that it doesn't make sense (for many reasons)
to maintain these packages within the repository. For this reason, I have included a bootstrap PowerShell script that 
takes out all the headache out of the process. 

```powershell
./bootstrap.ps1
```

This script will install vcpkg and its dependencies.

### Build Debug

With the bootstrap completed, you only have to run cmake. The beauty of CMake is the cascading effect it has and takes 
care of all the boilerplate out of it. Note my project might vary from your location. 

I would suggest running this from within the Developer Command Prompt from VS 2019, but you can (potentially) develop
on other terminals.

```powershell
cmake.exe -DCMAKE_BUILD_TYPE=Debug -G "CodeBlocks - NMake Makefiles" ./
cmake.exe --build ./cmake-build-debug --target all
```

### Build Release

You build release exactly like debug but only with the build type and folder changed.

```powershell
cmake.exe -DCMAKE_BUILD_TYPE=Release -G "CodeBlocks - NMake Makefiles" ./
cmake.exe --build ./cmake-build-release --target all
```

Happy dungeonin'!