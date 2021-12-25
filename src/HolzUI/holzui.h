#ifndef HOLZUI_H
#define HOLZUI_H

#include <stdint.h>
#include <cas-sdk/display.h>

typedef struct menu_item
{
	char name[40];
	uint8_t selected;
	void (*item_function)();
} menu_item;

typedef struct app_menu_item
{
    char name[40];
    char author[40];
    char version[40];
    char path[255];
    uint8_t selected;
    void (*item_function)();
} app_menu_item;

void print_string_fixed(char *string, uint8_t x_position, uint8_t y_position, cas_color foreground_color, cas_color background_color);
void print_char_fixed(char character, uint8_t x_position, uint8_t y_position, cas_color foreground_color, cas_color background_color);
void draw_app_info_box(char *app_name, char *app_author, char *app_version);
void draw_list_menu_item(menu_item item, uint8_t index, uint8_t selected_animation_status, cas_color foreground_color);
void draw_list_app_menu_item(app_menu_item item, uint8_t index, uint8_t selected_animation_status, cas_color foreground_color);

#endif