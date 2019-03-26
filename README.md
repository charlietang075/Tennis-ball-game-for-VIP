# Overview

This project is a motion detection computer game which is based on a tennis ball game. The game is suitable for 1 to 2 player(s), which mainly focus on servicing the elderly VIP. Four loudspeakers are implemented in four corners (Upper left, Upper right, Lower left, Lower right) and two webcams are set in opporsite direction, which are used to detect the motion of the players. During the game, a tennis ball sound will be generated from one speaker randomly; players have to identify the source of the sound and put their hands to the corresponding area. After that, the webcams will analysis the movement of the players then determine the correctness of them. If the player does the movement correctly, the scores will be increased by one in each round. After few rounds of the game, it will announce the winner and the scores of each player.

Releases can be found on the release list.

## System Requirment

### Hardware:
- USB 5.1 Channel Sound Card Adapter
- 4 Loudspeakers
- 2 Webcams

### Software:
- OpenCV 3.3.0
- Fmod API - fmod64.dll
- Visual Studio 2015

## Setup:

#### One player mode
Four Speakers in four corners and one webcamsin the center 

<img src="https://i.imgur.com/UlKfSiJ.jpg" width="230" height="300"> 
