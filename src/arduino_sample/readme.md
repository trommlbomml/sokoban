# Sample port to arduino

Here is a sample where I ported the game from the simulation to an arduino. My hardware:

* Elegoo 2,8'' Display 320x240 with 16-Bit Color support
* Arduino MEGA 2560
* Keyes_SJoys Joystick

The display was a simple shield I can plug into my arduino. The Joystick has 5 inputs (GRND, 5V, Analog-X, Analog-Y, Digital Switch Pin).

The display library is from Elegoo and a modification of the ADAFruit_GFX library.

## Port steps I have done

* copy the `src/sfml/sokoban/sokoban.cpp` content to arduino ide
* followed steps of `readme.md` from root folder
* included graphics library from example code
* replaced tft->drawPixel with tft.drawPixel()
* implemented key functions by:
  * have a flag for each input value
  * return flags in key_*_down
  * read values in loop and assign to flag variables
  