
#include "pch.h"
#include "VirtualDisplay.h"

VirtualDisplay::VirtualDisplay(sf::RenderWindow &window, int16_t w, int16_t h):
	_window(window),
	_width(w),
	_height(h),
	_pixels(nullptr)
{
	_pixels = new int16_t[w*h];
	for (auto i = 0; i < w*h; i++) _pixels[i] = 0;
}

VirtualDisplay::~VirtualDisplay()
{
	delete[] _pixels;
}

void VirtualDisplay::drawPixel(int16_t x, int16_t y, uint16_t color) 
{
	if (x < 0 || x >= _width || y < 0 || y >= _height) return;

	_pixels[y*_width + x] = color;
}

void VirtualDisplay::display() 
{
	sf::Event event;
	while (_window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			_window.close();
	}

	auto size = _window.getSize();

	auto pixelSize = std::min(size.x / _width, size.y / _height);
	auto offsetX = (size.x - pixelSize * _width) / 2;
	auto offsetY = (size.y - pixelSize * _height) / 2;

	_shape.setSize(sf::Vector2f(pixelSize,pixelSize));

	_window.clear(sf::Color::Blue);
	for (auto x = 0; x < _width; x++) 
	{
		for (auto y = 0; y < _height; y++) 
		{
			_shape.setPosition(sf::Vector2f(offsetX + x * pixelSize, offsetY + y * pixelSize));

			auto color = _pixels[y*_width + x];
			auto colorBits = rgb16_to_rgb32(color);
			_shape.setFillColor(sf::Color(colorBits));

			_window.draw(_shape);
		}
	}
	_window.display();
}

unsigned long VirtualDisplay::rgb16_to_rgb32(unsigned short a)
{
	unsigned long r = (a & 0xF800) >> 11;
	unsigned long g = (a & 0x07E0) >> 5;
	unsigned long b = (a & 0x001F);
	r = std::lroundf((float)r / 31.0f * 255.0f);
	g = std::lroundf((float)g / 63.0f * 255.0f);
	b = std::lroundf((float)b / 31.0f * 255.0f);
	return 0x000000ff | (r << 24) | (g << 16) | (b << 8);
}