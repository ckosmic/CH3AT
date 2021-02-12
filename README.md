# CH3AT

CH3AT is a Skate 3 trainer for RPCS3.  This has been tested on both the BLUS30464 and BLES00760 versions of the game.  CH3AT pulls addresses from the included Cheat Engine table, so most* other Skate 3 cheat tables will be compatible. 

\* Only Byte, 2 Byte Big Endian, 4 Byte Big Endian, Float Big Endian, and String values have been implemented so far. Sub-cheats are also ignored at the moment.

## Installation

Download the latest release from the [Releases](https://github.com/ckosmic/CH3AT/releases) tab. Extract the folder anywhere and run CH3AT.exe while in-game.

## Usage

- Open CH3AT when Skate 3 has been launched in RPCS3
- Press the ` or ~ key to open the CH3AT menu
- Click one of the textboxes to change a value
- Hit enter once you've typed a value in
- Press the ` or ~ key to close the CH3AT menu
- Scroll up and down to see more values with the scroll wheel or the up/down arrow keys
- Restore a default value by middle mouse clicking a textbox

## Building
- Download and extract the latest [SDL2_ttf development library](https://www.libsdl.org/projects/SDL_ttf/) and place it in the folder where CH3AT.sln is located.  Rename it to "SDL2_ttf".
- Download and extract the latest [SDL2 development library](https://www.libsdl.org/download-2.0.php) and place it in the folder where CH3AT.sln is located.  Rename it to "SDL2".
- Open the solution in Visual Studio 2019 and build.

## License
[MIT](https://choosealicense.com/licenses/mit/)