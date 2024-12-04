# Klei Adobe Tools

Klei Adobe Tools is a lightweight dynamic plugin for Adobe Animate to allow converting Klei formatted binary data (Anims, Builds, Textures) into an XFL project. Including the other way around to convert a FLA/XFL project (if properly formatted) into Klei formatted binary data for use in Klei game development.

**Currently, the library mainly supports the Don't Starve/Together data formats. Other formats are currently WIP**

## Installation

Download the `kadobe.dll` file from Releases and at the folder location of your Adobe Animate configurations folder, place in `Configuration/External Libraries`

On a Windows computer, the full path likely looks something like:
```
{DRIVE}:\Users\{USER_NAME}\AppData\Local\Adobe\Animate {VERSION}\{LANGUAGE_CODE}\Configuration\External Libraries
```
The dynamic library should now be properly installed.

Download the several `.jsfl` extension files from Releases and as you did above in the Adobe Animate configurations folder, place in `Configuration/Commands`

On a Windows computer, the full path likely looks something like:
```
{DRIVE}:\Users\{USER_NAME}\AppData\Local\Adobe\Animate {VERSION}\{LANGUAGE_CODE}\Configuration\Commands
```

Download the Don't Starve Mod Tools on steam. This is a temporary dependancy as we piggyback off these tools for compilation. It will be phased out once the library the 
library has it's own compilation functions implemented. This is only necessacary if you wish to compile, decompiling will work without the DS Mod Tools.

## Usage

The `.jsfl` scripts are the main way to interact and make use of the dynamic library. You too can develop your own `.jsfl` scripts if you wish! It is JavaScript with Flash's API extended into it. Here is an unfortunately outdated, but latest [documentation](https://help.adobe.com/archive/en_US/flash/cs5/flash_cs5_extending.pdf) on the API.

The following commands and scripts can be accessed within the `Commands` menu in Adobe Animate

Using the `Decompile{KLEI_GAME}Format` script will prompt you to select the zipped data of the respective games data and then select an out path to place the XFL project.

Using the `Compile{KLEI_GAME}Format` script will compile the CURRENTLY OPEN Animate project (FLA/XFL) into the respective games format, provided the project is properly formatted for compilation.

## Building From Source

NOTE: This project will never support Linux as Adobe Animate is not distributed to Linux systems

This project requires the following:
+ The build system generator used: [CMake](http://www.cmake.org), Minimum version 3.2
+ [ImageMagick 7.1.1 Q16 HDRI x64](https://imagemagick.org/archive/binaries/ImageMagick-7.1.1-41-Q16-HDRI-x64-dll.exe)
+ Platform's respective IDE
    + Visual Studio (Windows)
    + ~~XCode (Mac)~~ *Mac Support not yet tested!*

### Windows
Create a folder named `build` within project directory, and open command prompt with directoy changed to the newly created `build` folder and execute the following command:
```
cmake ../ -A x64
```

Open `KFormatLib.sln` and select `Release` as your active solution configuration. Then build the project (F7)

### Mac
+ TODO, Some day!