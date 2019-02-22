/*
Little game programmed for arduino with adafruit. because this is implemented with sfml, the code needs some
changes:

- remove sleep() method from this file
- remove includes
- add adafruit and arduino includes
- comment in while true in loop()
- remove main function
- replace implementations of key_* functions
- inject real adafruit gfx implementation and possibly replace drawPixel() calls
- have fun!

*/

//-----------------------------------------------------------------------------------------------
// Mock-implementations - Replace with Arduino ones
//-----------------------------------------------------------------------------------------------

#include "pch.h"

Adafruit_GFX *tft = nullptr;

//remove this function when running on arduino
void sleep(int32_t milliseconds)
{
	sf::sleep(sf::milliseconds(milliseconds));
}

//replace with arduino implementation
bool key_left_down()
{
	return sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A);
}

//replace with arduino implementation
bool key_right_down()
{
	return sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D);
}

//replace with arduino implementation
bool key_up_down()
{
	return sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W);
}

//replace with arduino implementation
bool key_down_down()
{
	return sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S);
}

//replace with arduino implementation
bool key_action_down()
{
	return sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space);
}

//-----------------------------------------------------------------------------------------------
// GAME Implementation
//-----------------------------------------------------------------------------------------------

const int16_t WIDTH = 21;
const int16_t HEIGHT = 14;

const int16_t FRAME_TIME_MILLISECONDS = 1000 / 25;
const int16_t ANIMATION_SPEED_LEVEL_WINNING_MILLSECONDS = 1000 / 12;
const int16_t ANIMATION_SPEED_GAME_WINNING_MILLSECONDS = 1000 / 20;
const int16_t SHOW_WINNING_SCREEN_MILLISECONDS = 2000;

//avoid dynamic allocation - increase if more targets needed.
const int16_t MAX_TARGET_COUNT = 10;

//colors for pixels.
const int16_t COLOR_EMPTY = 0x0000;
const int16_t COLOR_WALL = 0x738E;
const int16_t COLOR_BOX = 0x4800;
const int16_t COLOR_TARGET = 0x5D39;
const int16_t COLOR_PLAYER = 0x3CEA;

const int16_t COLOR_BLACK = 0x0000;
const int16_t COLOR_WHITE = 0xFFFF;
const int16_t COLOR_GREEN = 0x65ED;
const int16_t COLOR_YELLOW = 0xE782;

//Block definitions
const int8_t E = 0; // empty
const int8_t W = 1; // wall
const int8_t S = 2; // Start player
const int8_t B = 3; // box
const int8_t T = 4; // target for box

const int8_t STATE_GAME = 0;
const int8_t STATE_FINISH_LEVEL = 1;
const int8_t STATE_FINISH_GAME = 2;

//level definitions
const int16_t LEVEL_COUNT = 2;
const int8_t level_data[LEVEL_COUNT][WIDTH*HEIGHT] = {
	{
		E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,
		E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,
		E,E,E,E,E,E,W,W,W,W,W,E,E,E,E,E,E,E,E,E,E,
		E,E,E,E,E,E,W,E,E,E,W,E,E,E,E,E,E,E,E,E,E,
		E,E,E,E,E,E,W,E,B,E,W,E,W,W,W,E,E,E,E,E,E,
		E,E,E,E,E,E,W,E,B,E,W,E,W,B,W,E,E,E,E,E,E,
		E,E,E,E,E,E,W,W,W,S,W,W,W,T,W,E,E,E,E,E,E,
		E,E,E,E,E,E,E,W,W,E,E,E,E,T,W,E,E,E,E,E,E,
		E,E,E,E,E,E,E,W,E,E,E,W,E,E,W,E,E,E,E,E,E,
		E,E,E,E,E,E,E,W,E,E,E,W,W,W,W,E,E,E,E,E,E,
		E,E,E,E,E,E,E,W,W,W,W,W,E,E,E,E,E,E,E,E,E,
		E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,
		E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,
		E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E
	},
	{
		E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,
		E,E,E,E,E,W,W,W,W,W,E,E,E,E,E,E,E,E,E,E,E,
		E,E,E,E,E,W,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,
		E,E,E,E,E,W,B,E,B,W,E,E,E,E,E,E,E,E,E,E,E,
		E,E,E,W,W,W,E,E,E,W,W,E,E,E,E,E,E,E,E,E,E,
		E,E,E,W,E,E,E,B,E,S,W,E,E,E,E,E,E,E,E,E,E,
		E,W,W,W,E,W,E,W,W,E,W,E,E,E,W,W,W,W,W,W,E,
		E,W,E,E,E,W,E,W,W,E,W,W,W,W,W,E,E,T,B,W,E,
		E,W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,T,B,W,E,
		E,W,W,W,W,W,E,W,W,W,E,W,E,W,W,E,E,T,B,W,E,
		E,E,E,E,E,W,E,E,E,E,E,W,W,W,W,W,W,W,W,W,E,
		E,E,E,E,E,W,W,W,W,W,W,W,E,E,E,E,E,E,E,E,E,
		E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,
		E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,
	}
};

const int8_t winnig_level_screen[WIDTH*HEIGHT] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,
	0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,
	0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,
	0,0,0,0,1,0,0,1,0,0,0,0,0,1,0,0,1,0,0,0,0,
	0,0,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,0,0,
	0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,
	0,0,0,0,1,0,1,0,0,0,0,0,0,0,1,0,1,0,0,0,0,
	0,0,0,0,1,0,0,1,0,0,0,0,0,1,0,0,1,0,0,0,0,
	0,0,0,0,1,0,0,0,1,1,1,1,1,0,0,0,1,0,0,0,0,
	0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,
	0,0,0,0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

const int8_t winning_game_screen[WIDTH*HEIGHT] = {
	1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,1,
	0,0,0,0,0,0,0,2,2,2,2,2,2,2,0,0,0,0,0,0,0,
	1,0,0,0,0,0,0,0,2,2,2,2,2,0,0,0,0,0,0,0,1,
	0,0,0,0,0,0,2,0,2,2,2,2,2,0,2,0,0,0,0,0,0,
	1,0,0,0,0,0,0,2,2,2,2,2,2,2,0,0,0,0,0,0,1,
	0,0,0,0,0,0,0,0,2,2,2,2,2,0,0,0,0,0,0,0,0,
	1,0,0,0,0,0,0,0,0,2,2,2,0,0,0,0,0,0,0,0,1,
	0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,
	1,0,0,0,0,0,0,0,0,2,2,2,0,0,0,0,0,0,0,0,1,
	0,0,0,0,0,0,0,0,2,2,2,2,2,0,0,0,0,0,0,0,0,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0
};

struct position_t 
{
	int16_t x;
	int16_t y;
};

int8_t game_state;
int8_t current_level[WIDTH*HEIGHT];
int16_t current_level_index;
position_t targets[MAX_TARGET_COUNT];
int16_t target_count;

position_t player;
bool was_key_pressed;

int16_t paint_delta;
int16_t paint_elapsed_time;

void load_level(uint16_t index);

void clear_screen();
void draw_targets();
void draw_map();
void draw_player();
void clear_player();
void draw_box(int16_t x, int16_t y);
void draw_palette_pixel(int16_t x, int16_t y);

void update_game(int16_t elapsedMillis);
void update_finished_level(int16_t elapsedMillis);
void update_finished_game(int16_t elapsedMillis);

void handle_level_finished();

int8_t get_tile_at(int16_t x, int16_t y);
void set_tile_at(int16_t x, int16_t y, int8_t value);

int8_t is_free_to_move(int16_t x, int16_t y);
int8_t is_box(int16_t x, int16_t y);

void setup()
{
	game_state = STATE_GAME;
	current_level_index = 0;
	player.x = 0;
	player.y = 0;

	load_level(current_level_index);
}

void loop()
{
	//while (true)
	//{
	switch (game_state)
	{
	case STATE_GAME:
		update_game(FRAME_TIME_MILLISECONDS);
		break;
	case STATE_FINISH_LEVEL:
		update_finished_level(FRAME_TIME_MILLISECONDS);
		break;
	case STATE_FINISH_GAME:
		update_finished_game(FRAME_TIME_MILLISECONDS);
		break;
	}

	sleep(FRAME_TIME_MILLISECONDS);
	//}
}


void load_level(uint16_t index)
{
	clear_player();
	target_count = 0;
	for (int16_t i = 0; i < WIDTH*HEIGHT; ++i)
	{
		int8_t field = level_data[index][i];

		switch (field)
		{
		case S:
			player.x = i % WIDTH;
			player.y = i / WIDTH;
			field = E;
			break;
		case T:
			targets[target_count].x = i % WIDTH;
			targets[target_count].y = i / WIDTH;
			target_count++;
			field = E;
			break;
		}
		current_level[i] = field;
	}

	was_key_pressed = false;
	draw_map();
	draw_targets();
	draw_player();
}

void draw_targets()
{
	for (int16_t i = 0; i < target_count; i++)
	{
		int8_t field = get_tile_at(targets[i].x, targets[i].y);
		if (field != B && (targets[i].x != player.x || targets[i].y != player.y))
		{
			tft->drawPixel(targets[i].x, targets[i].y, COLOR_TARGET);
		}
	}
}

void draw_player()
{
	tft->drawPixel(player.x, player.y, COLOR_PLAYER);
}

void clear_player() 
{
	tft->drawPixel(player.x, player.y, COLOR_EMPTY);
}

void draw_box(int16_t x, int16_t y)
{
	tft->drawPixel(x, y, COLOR_BOX);
}

void draw_map()
{
	for (int16_t y = 0; y < HEIGHT; y++)
	{
		for (int16_t x = 0; x < WIDTH; x++)
		{
			switch (current_level[y*WIDTH + x])
			{
			case E:
				tft->drawPixel(x, y, COLOR_EMPTY);
				break;
			case W:
				tft->drawPixel(x, y, COLOR_WALL);
				break;
			case B:
				tft->drawPixel(x, y, COLOR_BOX);
				break;
			}
		}
	}
}

int8_t get_tile_at(int16_t x, int16_t y)
{
	return current_level[y*WIDTH + x];
}

void set_tile_at(int16_t x, int16_t y, int8_t value)
{
	current_level[y*WIDTH + x] = value;
}

int8_t is_free_to_move(int16_t x, int16_t y)
{
	int8_t tile = get_tile_at(x, y);
	return tile == E || tile == T;
}

int8_t is_box(int16_t x, int16_t y)
{
	return get_tile_at(x, y) == B;
}

void clear_screen()
{
	for (int16_t x = 0; x < WIDTH; x++)
	{
		for (int16_t y = 0; y < HEIGHT; y++)
		{
			tft->drawPixel(x, y, 0x0000);
		}
	}
}

void handle_level_finished() 
{
	bool all_boxes_placed = true;

	for (int16_t i = 0; i < target_count; i++)
	{
		if (get_tile_at(targets[i].x, targets[i].y) != B)
		{
			all_boxes_placed = false;
			break;
		}
	}

	if (all_boxes_placed) 
	{
		paint_delta = 0;
		paint_elapsed_time = 0;
		if (current_level_index == LEVEL_COUNT - 1) 
		{
			game_state = STATE_FINISH_GAME;
		}
		else
		{
			game_state = STATE_FINISH_LEVEL;
		}
	}
}

void update_game(int16_t elapsedMillis)
{
	bool current_key_pressed = true;
	bool load_new_level = false;

	position_t target_player;
	position_t target_box;

	target_player.x = player.x;
	target_player.y = player.y;
	target_box.x = player.x;
	target_box.y = player.y;

	if (key_left_down())
	{
		if (!was_key_pressed)
		{
			target_player.x -= 1;
			target_box.x -= 2;
		}
	}
	else if (key_right_down())
	{
		if (!was_key_pressed)
		{
			target_player.x += 1;
			target_box.x += 2;
		}
	}
	else if (key_up_down()) 
	{
		if (!was_key_pressed)
		{
			target_player.y -= 1;
			target_box.y -= 2;
		}
	}
	else if (key_down_down()) 
	{
		if (!was_key_pressed)
		{
			target_player.y += 1;
			target_box.y += 2;
		}
	}
	else if (key_action_down())
	{
		load_new_level = true;
	}
	else
	{
		current_key_pressed = false;
	}

	if (load_new_level)
	{
		load_level(current_level_index);
	}
	else if (target_player.x != player.x || target_player.y != player.y)
	{
		if (is_free_to_move(target_player.x, target_player.y)
			|| is_box(target_player.x, target_player.y) && is_free_to_move(target_box.x, target_box.y))
		{
			clear_player();
			player.x = target_player.x;
			player.y = target_player.y;
			draw_player();

			if (is_box(target_player.x, target_player.y)) 
			{
				draw_box(target_box.x, target_box.y);
				set_tile_at(target_box.x, target_box.y, B);
				set_tile_at(target_player.x, target_player.y, E);

				handle_level_finished();
			}
		}

		draw_targets();
	}
	
	was_key_pressed = current_key_pressed;
}

void update_finished_level(int16_t elapsedMillis)
{
	if (paint_delta == WIDTH)
	{
		if (key_action_down())
		{
			++current_level_index;
			load_level(current_level_index);
			game_state = STATE_GAME;
		}
		return;
	}

	paint_elapsed_time += elapsedMillis;

	if (paint_elapsed_time >= ANIMATION_SPEED_LEVEL_WINNING_MILLSECONDS)
	{
		paint_elapsed_time -= ANIMATION_SPEED_LEVEL_WINNING_MILLSECONDS;

		for (int16_t y = 0; y < HEIGHT; y++)
		{
			int16_t x = y % 2 == 0 ? paint_delta : WIDTH - paint_delta - 1;
			int16_t color = winnig_level_screen[y*WIDTH + x] == 0 ? COLOR_WHITE : COLOR_GREEN;

			tft->drawPixel(x, y, color);
		}

		++paint_delta;
	}
}

void draw_palette_pixel(int16_t x, int16_t y)
{
	int8_t palette_color = winning_game_screen[y*WIDTH + x];

	int16_t color = palette_color == 0 ? COLOR_WHITE : palette_color == 1 ? COLOR_GREEN : COLOR_YELLOW;

	tft->drawPixel(x, y, color);
}

void update_finished_game(int16_t elapsedMillis)
{
	if (paint_delta == (WIDTH / 2 + 1)*HEIGHT) 
	{
		if (key_action_down())
		{
			current_level_index = 0;
			load_level(current_level_index);
			game_state = STATE_GAME;
		}

		return;
	}

	paint_elapsed_time += elapsedMillis;

	if (paint_elapsed_time >= ANIMATION_SPEED_GAME_WINNING_MILLSECONDS)
	{
		paint_elapsed_time -= ANIMATION_SPEED_GAME_WINNING_MILLSECONDS;

		int16_t x1 = paint_delta / HEIGHT;
		int16_t x2 = WIDTH - x1 - 1;
		int16_t y = paint_delta % HEIGHT;
		if (x1 % 2 == 1)
		{
			y = HEIGHT-1 - y;
		}

		draw_palette_pixel(x1, y);
		draw_palette_pixel(x2, y);

		++paint_delta;
	}
}

//-----------------------------------------------------------------------------------------------
// Remove this function when running on arduino
//-----------------------------------------------------------------------------------------------

int main()
{
	sf::RenderWindow window(sf::VideoMode(800, 600), "SFML works!");

	std::unique_ptr<VirtualDisplay> local_tft = std::make_unique<VirtualDisplay>(window, WIDTH, HEIGHT);
	tft = local_tft.get();

	setup();

	while (window.isOpen())
	{
		loop();
		local_tft->display();
	}

	return 0;
}