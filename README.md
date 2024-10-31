# Klei Adobe Tools

Klei Adobe Tools is a lightweight dynamic library for Adobe Animate to allow converting Klei formatted binary data (Anims, Builds, Textures) into an XFL project. Including the other way around to convert a FLA/XFL project (if properly formatted) into Klei formatted binary data for use in Klei games.

**Currently, the library mainly supports the Don't Starve/Together data formats. Other formats are currently WIP**

## Installation

Download the `kadobe.dll` file from Releases and at the folder location of your Adobe Animate configurations folder, place in `Configuration/External Libraries`

On a Windows computer, the full path likely looks something like:
```C:\Users\{USER_NAME}\AppData\Local\Adobe\Animate {VERSION}\{LANGUAGE_CODE}\Configuration\External Libraries```
The dynamic library should now be properly installed.

Download the several `.jsfl` extension files from Releases and as you did above in the Adobe Animate configurations folder, place in `Configuration/Commands`

On a Windows computer, the full path likely looks something like:
```C:\Users\{USER_NAME}\AppData\Local\Adobe\Animate {VERSION}\{LANGUAGE_CODE}\Configuration\Commands```
This step is not technically nessacary as you can run any command from any path in Adobe Animate. However placing these scripts here will make them easy to access as part of Animate's UI, conveniencing you so that you do not have to open File Explorer.

## Usage

The `.jsfl` scripts are the main way to interact and make use of the dynamic library. You too can develop your own `.jsfl` scripts if you wish! It is JavaScript with Flash's API extended into it.

Using the `Decompile{KLEI_GAME}Format` script will prompt you to select the zipped data of the respective games data and then select an out path to place the XFL project.

Using the `Compile{KLEI_GAME}Format` script will compile the CURRENTLY OPEN Animate project (FLA/XFL) into the respective games format.

## Building From Source

TODO :)
