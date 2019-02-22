# Sokoban clone for arduino

This is a little, simple clone of sokoban in C++. The game is implemented in simple c-style to be able to run on arduino devices.

## Project Structure

* `editor` - tooling and documentation to create levels
* `src` - source code of the game
  * `sfml` - implementation with sfml for desktop testing

## Editing the game

Update the game using the sfml solution with Visual Studio 2017. You need to install and configure sfml properly, see official documentation:
[SFML with Visual Studio](https://www.sfml-dev.org/tutorials/2.5/start-vc.php)

## Porting the game to arduino

You need to make several modifications to the game to make it work on Arduino:

* create a new sketch file from the arduino IDE
* copy all the content from `src/sfml/sokoban/sokoban.cpp` to your sketch
* from there edit the sketch content:
  * remove `#include "pch.h"`
  * remove sleep() function
  * implement methods key_*down() according to your arduino wirings
  * comment in the code inside the loop() function
  * remove main function
  * implement the display by using an adafruit_GFX implementation of your choice; only drawPixel() is used from the interface.