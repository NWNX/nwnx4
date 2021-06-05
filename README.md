

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

### Build Debug/Release

With the bootstrap completed, you only have to run Meson. Good thing is that I took out the headache out of this too.
From a VS Command Prompt, enter the following command:

```powershell
pwsh ./build.ps1
```

You should find two folders: meson-build-debug and meson-build-release. They are the built version of the codebase respectively divided upon environments.