# Tennis-inspired Sound-Only Game for Visually Impaired People
[中文遊戲設置按此](https://github.com/charlietang075/Tennis-ball-game-for-VIP/blob/master/Chinese_instruction.md）
## Overview
This project is a simple sound-only motion game inspired by Tennis, where players try to gain as many points as possible by identifying the source of sound between 4 speakers in turns. Players give their answers to the computer via motion detection using a webcam/related hardware. 

Supports 1-2 players.
## System Requirements
A compatible computer with minimum 4 speakers and 1 webcam (2 for 2 player) is required. 

We have tested the game on Windows using an external 5.1 channel sound card 4 speakers, and 1-2 Webcams.

Only Windows is officially supported at the moment.

The 4 speakers and 1-2 webcams are placed in the following example configuration (Upper left/right, lower left/right):
<img src="https://i.imgur.com/UlKfSiJ.jpg" width="230" height="300"> 

The fixture should be placed such that player(s) should each face a webcam and stand in the centre of the webcam’s view. 
## How to play
Head over to [Releases](https://github.com/charlietang075/Tennis-ball-game-for-VIP/releases) and grab the latest release and follow instructions to setup the game.

Press “G” and Enter to start the game after the initial voice announcement.

The game will now automatically run. During each turn, one speaker will make a sound. The player should raise his hand in response to the physical location of the speaker. The game will then make a corresponding sound to indicate if the answer is correct or incorrect.

At the end of the game (10 turns), the score will be announced and the game will automatically quit.
## Build and Dependencies
Our releases are built for x64 Windows using Visual Studio 2015.

The following dependencies are required for the game to function:
- [OpenCV 3.3.0](https://opencv.org/releases.html)
- [FMOD Studio API](https://www.fmod.com/download)

The built program use DLLs (specifically, opencv_world330.dll and fmodL64.dll) to function which we do not include in our release or source. You may find these libraries available in the links above.
