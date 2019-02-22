#pragma once

#include "Adafruit_GFX.h"

class VirtualDisplay : public Adafruit_GFX 
{
private:

	sf::RenderWindow& _window;
	sf::RectangleShape _shape;
	int _width;
	int _height;
	int16_t* _pixels;

public:
	VirtualDisplay(sf::RenderWindow &window, int16_t w, int16_t h);
	~VirtualDisplay();

public:

	void drawPixel(int16_t x, int16_t y, uint16_t color);

	void display();

private:

	unsigned long rgb16_to_rgb32(unsigned short a);
};