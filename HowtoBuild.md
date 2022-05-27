# Open Here - How to Build

## Prerequisites
* Visual Studio 2022 (or newer) - Community Edition or higher
* Visual C++ Compiler Tools 2022 v143
* Windows SDK 10.0 (latest version)
* C++ Windows Desktop Development Preset in Visual Studio Installer recommended

## Building
* Open the `open-here.sln" Visual Studio Solution.
* Select Desired Platform, e.g. "Release, x64".
* Build Solution.
* Dependencies among projects within the solution should be resolved automatically.

## Results
The target directory is: `.\bin\{PlatformName}\{Configuration}`

For example: `.\bin\x64\Release`

Within you will find the executable files, and optionally the debugging symbol files (*.pdb).

## Continuous Integration on Github
The continuous integration on Github, using Github actions, defined in the subdirector `.\.github\workflows`, use MSBuild to compile the solution.

One difference is, the Github action injects the CI Build number as fourth number into the version number in `Version.h`:
```c
#define OPEN_HERE_VER_BUILD	0
```

## Tests

### Console Application
`.\ConsoleApp\runtest.ps1` performs a simple smoke test.
It opens Microsoft Windows File Explorer instances, and checks the results of `OpenHereCon.exe` for the correct paths being opened, and correct files being selected.
This test is part of the Github Actions CI.
