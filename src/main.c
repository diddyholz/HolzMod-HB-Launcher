#include <string.h>
#include <stdint.h>
#include <cas-sdk/display.h>
#include <cas-sdk/debug.h>
#include <cas-sdk/input/key-input.h>
#include <cas-sdk/file-system.h>
#include "HolzUI/holzui.h"

#define MAXAPPBUFFER 0x00100000

#define HMB_ADDRESS_LOAD 0x8CA00000

#define HMB_OFFSET_AUTHOR 0x28
#define HMB_OFFSET_VERSION 0x50
#define HMB_OFFSET_BINARY 0x80

#define HOLZMOD_VER_ADDRESS 0x814FFFC0
#define HOLZMOD_EXCEPTION_HANDLER 0x80EB5000 // currently unused

#define EXPEVT_ADDRESS 0xFF000024

void menu_load_program();
void menu_about();
void menu_settings();
void print_header();
void byte_to_string(uint8_t byte, char *strBuffer);
void convert_wchar_to_char(uint16_t *src, uint8_t *dst, uint32_t size);

void main()
{
	// set vrb to custom exception handler (currently disabled)
	// asm volatile (
	//   "mov %0, r0;"
	//   "ldc r0, vbr;"
	//   : 
	//   : "r" (holzmod_exception_handler)
	//   : "r0"
	// );

	// write version info
	int32_t ver_file = fs_file_open("/fls0/version.txt", OPEN_CREATE | OPEN_WRITE);

	if (ver_file > 0)
	{
		fs_file_write(ver_file, (char *)HOLZMOD_VER_ADDRESS, 0x20);
		fs_file_close(ver_file);
	}

	// populate menu
	menu_item menu_items[3];

	strcpy(menu_items[0].name, "Load program");
	menu_items[0].selected = 1;
	menu_items[0].item_function = menu_load_program;

	strcpy(menu_items[1].name, "HolzMod settings");
	menu_items[1].selected = 0;
	menu_items[1].item_function = menu_settings;

	strcpy(menu_items[2].name, "About HolzMod");
	menu_items[2].selected = 0;
	menu_items[2].item_function = menu_about;

	uint8_t selected_animation_stage = 10;
	uint8_t animation_going_up = 0;

	uint8_t selected_menu_item = 0;
	uint8_t button_pressed = 1;

	cas_key_input key_input;
	input_get_keys(&key_input);

	cas_color background = { .as_uint16 = 0x0000 };

	cas_color foreground = { .as_uint16 = 0xFFFF };

	uint16_t counter = 0;

	// background
	display_fill(background);

	// print header
	print_header();

	while (1)
	{
		// draw menu
		for (uint8_t i = 0; i < 3; i++)
		{
			draw_list_menu_item(menu_items[i], i, selected_animation_stage, foreground);
		}

		if (animation_going_up)
		{
			selected_animation_stage += 1;
			if (selected_animation_stage == 10)
				animation_going_up = 0;
		}
		else
		{
			selected_animation_stage -= 1;
			if (selected_animation_stage == 0)
				animation_going_up = 1;
		}

		display_refresh();

		input_get_keys(&key_input);

		if (button_pressed && (key_input.buffer_one | key_input.buffer_two) > 0)
			continue;

		button_pressed = 0;

		// handle button input
		if ((key_input.buffer_two & KEY_UP_2) > 0)
		{
			if (selected_menu_item != 0)
			{
				menu_items[selected_menu_item].selected = 0;
				selected_menu_item--;
				menu_items[selected_menu_item].selected = 1;

				selected_animation_stage = 10;
				animation_going_up = 0;
			}

			button_pressed = 1;
		}

		if ((key_input.buffer_two & KEY_DOWN_2) > 0)
		{
			if (selected_menu_item != 2)
			{
				menu_items[selected_menu_item].selected = 0;
				selected_menu_item++;
				menu_items[selected_menu_item].selected = 1;

				selected_animation_stage = 10;
				animation_going_up = 0;
			}

			button_pressed = 1;
		}

		if ((key_input.buffer_one & KEY_EXE_1) > 0)
		{
			menu_items[selected_menu_item].item_function();

			// background
			display_fill(background);

			// print header
			print_header();
		}

		if ((key_input.buffer_one & KEY_ON_CLEAR_1) > 0 && (key_input.buffer_one & KEY_SHIFT_1) > 0)
			break;
	}

	// restart cas by jumping to address 0 (crash)
	((void (*)(void))0)();
}

void menu_load_program()
{
	// fill background
	display_fill((cas_color){ 0x0000 });

	// print header
	print_header();

	cas_color foreground;
	foreground.red = 0b11111;
	foreground.green = 0;
	foreground.blue = 0;

	cas_color background;
	background.red = 0;
	background.green = 0;
	background.blue = 0;

	uint32_t selected_app = 0;
	uint32_t app_count = 0;

	uint8_t button_pressed = 1;

	uint8_t selected_animation_stage = 10;
	uint8_t animation_going_up = 0;

	app_menu_item apps[60];

	fs_init();

	// get all apps
	for (unsigned char x = 0; x < 60; x++)
	{
		// generate file name
		char app_path[255] = "/fls0/apps/app";
		char temp[0x80];

		byte_to_string(x, temp);
		strcat(app_path, temp);
		strcat(app_path, ".hmb");

		long file_handle = fs_file_open(app_path, OPEN_READ);

		if (file_handle < 0)
			break;

		fs_file_read(file_handle, temp, 0x80);

		strncpy(apps[x].name, temp, 40);
		strncpy(apps[x].author, &temp[HMB_OFFSET_AUTHOR], 40);
		strncpy(apps[x].version, &temp[HMB_OFFSET_AUTHOR], 40);
		strcpy(apps[x].path, app_path);

		if (x == 0)
			apps[x].selected = 1;
		else
			apps[x].selected = 0;

		fs_file_close(file_handle);

		app_count++;
	}

	if (app_count == 0)
	{
		print_string_fixed("No apps were found", 10, 16, foreground, background);
		print_string_fixed("Make sure you put the apps into", 5, 18, foreground, background);
		print_string_fixed("the apps directory of your cas", 5, 19, foreground, background);
		print_string_fixed("and name them app1, app2, app3, etc", 2, 20, foreground, background);
	}

	foreground.red = 0b11111;
	foreground.green = 0b111111;
	foreground.blue = 0b11111;

	while (1)
	{
		// draw all apps
		for (unsigned char x = 0; x < app_count; x++)
		{
			draw_list_app_menu_item(apps[x], x, selected_animation_stage, foreground);
		}

		if (animation_going_up)
		{
			selected_animation_stage += 1;
			if (selected_animation_stage == 10)
				animation_going_up = 0;
		}
		else
		{
			selected_animation_stage -= 1;
			if (selected_animation_stage == 0)
				animation_going_up = 1;
		}

		// draw app info box
		draw_app_info_box(apps[selected_app].name, apps[selected_app].author, apps[selected_app].version);

		display_refresh();

		// handle button input
		cas_key_input key_input;
		input_get_keys(&key_input);

		if (button_pressed && (key_input.buffer_one | key_input.buffer_two) > 0)
			continue;

		button_pressed = 0;

		// handle button input
		if ((key_input.buffer_two & KEY_UP_2) > 0)
		{
			if (selected_app != 0)
			{
				apps[selected_app].selected = 0;
				selected_app--;
				apps[selected_app].selected = 1;

				selected_animation_stage = 10;
				animation_going_up = 0;
			}

			button_pressed = 1;
		}

		if ((key_input.buffer_two & KEY_DOWN_2) > 0)
		{
			if (selected_app != (app_count - 1))
			{
				apps[selected_app].selected = 0;
				selected_app++;
				apps[selected_app].selected = 1;

				selected_animation_stage = 10;
				animation_going_up = 0;
			}

			button_pressed = 1;
		}

		if ((key_input.buffer_one & KEY_EXE_1) > 0)
		{
			cas_color highlight;
			highlight.red = 0b11111;
			highlight.green = 0;
			highlight.blue = 0;

			// display loading message
			display_fill(background);

			// print header
			print_header();

			char loading_string[40] = "Loading ";
			strcat(loading_string, apps[selected_app].name);

			print_string_fixed(loading_string, (40 - strlen(loading_string)) / 2, 18, highlight, background);

			display_refresh();

			// load the selected app into memory
			long file_handle = fs_file_open(apps[selected_app].path, OPEN_READ);

			if (file_handle < 0)
			{
				print_string_fixed("Error loading app", 11, 18, highlight, background);
				print_string_fixed("Press [EXE] to return", 9, 17, highlight, background);

				while ((key_input.buffer_one & KEY_EXE_1) > 0)
					input_get_keys(&key_input);

				button_pressed = 1;
			}
			else
			{
				fs_file_read(file_handle, (char *)HMB_ADDRESS_LOAD, MAXAPPBUFFER);
				fs_file_close(file_handle);

				// jump to app
				typedef int func(void);
				func *app_main = (func *)(HMB_ADDRESS_LOAD + HMB_OFFSET_BINARY);
				int app_return_value = app_main();
			}

			button_pressed = 1;

			// background
			display_fill(background);

			// print header
			print_header();
		}

		if ((key_input.buffer_one & KEY_BACKSPACE_1) > 0)
			return;

		if ((key_input.buffer_one & KEY_ON_CLEAR_1) > 0 && (key_input.buffer_one & KEY_SHIFT_1) > 0)
			break;
	}

	// restart cas by jumping to 0
	((void (*)(void))0)();
}

void menu_settings()
{
	uint8_t button_pressed = 1;

	// fill background
	display_fill((cas_color){ 0x0000 });

	// print header
	print_header();

	cas_color foreground;
	foreground.red = 0b11111;
	foreground.green = 0;
	foreground.blue = 0;

	cas_color background = { .as_uint16 = 0x0000 };

	print_string_fixed("Nothing here yet", 12, 18, foreground, background);

	display_refresh();

	while (1)
	{

		// handle button input
		cas_key_input key_input; 
		input_get_keys(&key_input);

		if (button_pressed && (key_input.buffer_one | key_input.buffer_two) > 0)
			continue;

		button_pressed = 0;

		if ((key_input.buffer_one & KEY_BACKSPACE_1) > 0)
			return;

		if ((key_input.buffer_one & KEY_ON_CLEAR_1) > 0 && (key_input.buffer_one & KEY_SHIFT_1) > 0)
			break;
	}

	// restart cas by jumping to 0
	((void (*)(void))0)();
}

void menu_about()
{
	uint8_t button_pressed = 1;

	// fill background
	display_fill((cas_color){ 0x0000 });

	// print header
	print_header();

	cas_color foreground;
	foreground.red = 0b11111;
	foreground.green = 0;
	foreground.blue = 0;

	cas_color background = { .as_uint16 = 0x0000 };

	print_string_fixed("*****************************", 5, 15, foreground, background);
	print_string_fixed("*                           *", 5, 16, foreground, background);
	print_string_fixed("* HolzMod Homebrew Launcher *", 5, 17, foreground, background);
	print_string_fixed("*                           *", 5, 18, foreground, background);
	print_string_fixed("*       (c) diddyholz       *", 5, 19, foreground, background);
	print_string_fixed("*                           *", 5, 20, foreground, background);
	print_string_fixed("*****************************", 5, 21, foreground, background);

	// print version
	print_string_fixed((char *)HOLZMOD_VER_ADDRESS, (40 - strlen((char *)HOLZMOD_VER_ADDRESS)) / 2, 35, foreground, background);

	display_refresh();

	while (1)
	{
		// handle button input
		cas_key_input key_input;
		input_get_keys(&key_input);

		if (button_pressed && (key_input.buffer_one | key_input.buffer_two) > 0)
			continue;

		button_pressed = 0;

		if ((key_input.buffer_one & KEY_BACKSPACE_1) > 0)
			return;

		if ((key_input.buffer_one & KEY_ON_CLEAR_1) > 0 && (key_input.buffer_one & KEY_SHIFT_1) > 0)
			break;
	}

	// restart cas by jumping to 0
	((void (*)(void))0)();
}

void print_header()
{
	cas_color foreground;
	foreground.red = 0b11111;
	foreground.green = 0b111111;
	foreground.blue = 0b11111;

	cas_color background = { .as_uint16 = 0x0000 };

	print_string("<<< HolzMod HB launcher >>>", 52, 2, 0, foreground, background, 0);

	foreground.red = 0b11111;
	foreground.green = 0;
	foreground.blue = 0;

	// draw a divider
	for (unsigned short x = 0; x < DISPLAY_WIDTH; x++)
	{
		display_framebuffer[16][x] = foreground;
	}
}

void convert_wchar_to_char(uint16_t *src, uint8_t *dst, uint32_t size)
{
	for (unsigned long x = 0; x < size; x++)
	{
		dst[x] = (unsigned char)src[x];
	}
}

void byte_to_string(uint8_t byte, char *str_buffer)
{
	// convert byte to decimal string
	unsigned char buffer_counter = 0;

	if (byte / 10 >= 10)
		buffer_counter = 2;
	if (byte / 10 < 10)
		buffer_counter = 1;
	if (byte / 10 == 0)
		buffer_counter = 0;

	// fill buffer with 0
	str_buffer[0] = 0;
	str_buffer[1] = 0;
	str_buffer[2] = 0;
	str_buffer[3] = 0;

	do
	{
		str_buffer[buffer_counter] = (byte % 10) + '0';

		byte /= 10;
		buffer_counter--;
	} while (byte > 0);
}

// Custom exception handler disabled for now
// void __attribute__((section(".exceptions"))) exception_handler()
// {
//   uint32_t expevt = *((uint32_t *)EXPEVT_ADDRESS);

//   display_fill((cas_color){ 0x0000 });
	
//   print_string("exception!", 7, 11, (cas_color){ 0xFFFF }, (cas_color){ 0x0000 });
//   print_hex_word(expevt >>16, 8, 12);
//   print_hex_word(expevt, 13, 12);
	
//   display_refresh();

//   for(;;) { }
// }

void main() __attribute__((section(".main")));
