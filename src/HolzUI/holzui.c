#include <stdint.h>
#include <cas-sdk/display.h>
#include <cas-sdk/debug.h>
#include "holzui.h"

#define FONTBASE 		0x80632fd0

#define CHARWIDTH 	8
#define CHARHEIGHT 	12

void print_char_fixed(char character, uint8_t x_position, uint8_t y_position, cas_color foreground_color, cas_color background_color)
{
	uint8_t char_index = character - ' ';

	uint16_t temp_y = y_position * (CHARHEIGHT + 2);
	uint16_t temp_x = x_position * CHARWIDTH;

	// fill background with background color
	for(uint8_t y = 0; y < CHARHEIGHT + 2; y++)
	{
		temp_x = x_position * CHARWIDTH;
		
		for(uint8_t x = 0; x < CHARWIDTH; x++)
		{
			display_framebuffer[temp_y][temp_x] = background_color;
			temp_x++;
		}

		temp_y++;
	}

	// now draw the character
	uint16_t *pixel = (unsigned short *)(FONTBASE + (0xC0 * char_index));

	temp_y = y_position * (CHARHEIGHT + 2) + 1;
	temp_x = x_position * CHARWIDTH;

	for(unsigned char y = 0; y < CHARHEIGHT; y++)
	{
		temp_x = x_position * CHARWIDTH;

		for(unsigned char x = 0; x < CHARWIDTH; x++)
		{
			if(*pixel == 0)
				display_framebuffer[temp_y][temp_x] = foreground_color;
			
			temp_x++;
			pixel++;
		}

		temp_y++;
	}
}

void print_string_fixed(char *string, uint8_t x_position, uint8_t y_position, cas_color foreground_color, cas_color background_color)
{
	uint32_t counter = 0;
	uint32_t line_length = DISPLAY_WIDTH / CHARWIDTH;

	while (string[counter] != 0)
	{
		print_char_fixed(string[counter], x_position, y_position + (x_position / line_length), foreground_color, background_color);  
		counter++; 
		x_position++;
	}    
}

void draw_list_menu_item(menu_item item, uint8_t index, uint8_t selected_animation_status, cas_color foreground_color)
{
	cas_color selected_color;

	switch (selected_animation_status)
	{
		case 0:
			selected_color.red = 0;
			selected_color.green = 0;
			selected_color.blue = 0;
			break;
		case 1:
			selected_color.red = 1;
			selected_color.green = 2;
			selected_color.blue = 1;
			break;
		case 2:
			selected_color.red = 1;
			selected_color.green = 5;
			selected_color.blue = 1;
			break;
		case 3:
			selected_color.red = 2;
			selected_color.green = 8;
			selected_color.blue = 2;
			break;
		case 4:
			selected_color.red = 4;
			selected_color.green = 12;
			selected_color.blue = 4;
			break;
		case 5:
			selected_color.red = 6;
			selected_color.green = 15;
			selected_color.blue = 6;
			break;
		case 6:
			selected_color.red = 8;
			selected_color.green = 18;
			selected_color.blue = 8;
			break;
		case 7:
			selected_color.red = 10;
			selected_color.green = 21;
			selected_color.blue = 10;
			break;
		case 8:
			selected_color.red = 12;
			selected_color.green = 24;
			selected_color.blue = 12;
			break;
		case 9:
			selected_color.red = 14;
			selected_color.green = 27;
			selected_color.blue = 14;
			break;
		case 10:
			selected_color.red = 15;
			selected_color.green = 30;
			selected_color.blue = 15;
			break;
	}
	
	if(!item.selected)
	{
		selected_color.red = 0;
		selected_color.green = 0;
		selected_color.blue = 0;
	}

	// print name
	print_string_fixed("                                        ", 0, 3 + index , foreground_color, selected_color); // first draw row full of spaces
	print_string_fixed(item.name, 1, 3 + index, foreground_color, selected_color);
}

void draw_app_info_box(char *app_name, char *app_author, char *app_version)
{
	cas_color accent_color = { .as_uint16 = 0xF800 };
	cas_color background_color = { .as_uint16 = 0x0000 };
	cas_color font_color = { .as_uint16 = 0xFFFF };

	// draw divider
	for(uint16_t x = 0; x < DISPLAY_WIDTH; x++)
		display_framebuffer[477][x] = accent_color;

	// draw app info
	char app_name_string[38] = "app name: \0";
	char app_author_string[38] = "app author: \0";
	char app_version_string[38] = "app version: \0";

	if(app_name[0] == 0)
		strcpy(app_name, "not specified");
	if(app_author[0] == 0)
		strcpy(app_author, "not specified");
	if(app_version[0] == 0)
		strcpy(app_version, "not specified");

	char space_array[39] = "                                       ";

	print_string(app_name_string, 8, 480, 0, accent_color, background_color, 0);
	print_string(space_array, 88, 480, 0, font_color, background_color, 0);
	print_string(app_name, 88, 480, 0, font_color, background_color, 0);

	print_string(app_author_string, 8, 496, 0, accent_color, background_color, 0);
	print_string(space_array, 104, 496, 0, font_color, background_color, 0);
	print_string(app_author, 104, 496, 0, font_color, background_color, 0);

	print_string(app_version_string, 8, 512, 0, accent_color, background_color, 0);
	print_string(space_array, 112, 512, 0, font_color, background_color, 0);
	print_string(app_version, 112, 512, 0, font_color, background_color, 0);
}

void draw_list_app_menu_item(app_menu_item item, uint8_t index, uint8_t selected_animation_status, cas_color foreground_color)
{
	cas_color selected_color;

	switch (selected_animation_status)
	{
		case 0:
			selected_color.red = 0;
			selected_color.green = 0;
			selected_color.blue = 0;
			break;
		case 1:
			selected_color.red = 1;
			selected_color.green = 2;
			selected_color.blue = 1;
			break;
		case 2:
			selected_color.red = 1;
			selected_color.green = 5;
			selected_color.blue = 1;
			break;
		case 3:
			selected_color.red = 2;
			selected_color.green = 8;
			selected_color.blue = 2;
			break;
		case 4:
			selected_color.red = 4;
			selected_color.green = 12;
			selected_color.blue = 4;
			break;
		case 5:
			selected_color.red = 6;
			selected_color.green = 15;
			selected_color.blue = 6;
			break;
		case 6:
			selected_color.red = 8;
			selected_color.green = 18;
			selected_color.blue = 8;
			break;
		case 7:
			selected_color.red = 10;
			selected_color.green = 21;
			selected_color.blue = 10;
			break;
		case 8:
			selected_color.red = 12;
			selected_color.green = 24;
			selected_color.blue = 12;
			break;
		case 9:
			selected_color.red = 14;
			selected_color.green = 27;
			selected_color.blue = 14;
			break;
		case 10:
			selected_color.red = 15;
			selected_color.green = 30;
			selected_color.blue = 15;
			break;
	}
	
	if(!item.selected)
	{
			selected_color.red = 0;
			selected_color.green = 0;
			selected_color.blue = 0;
	}

	// print name
	print_string_fixed("                                        ", 0, 3 + index , foreground_color, selected_color); // first draw full of spaces
	print_string_fixed(item.name, 1, 3 + index, foreground_color, selected_color);
}