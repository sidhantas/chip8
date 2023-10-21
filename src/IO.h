#pragma once
#include <ApplicationServices/ApplicationServices.h>
#include <stdbool.h>
void *enable_keyboard_raw_mode();
CGEventRef CGEventCallback(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void *refcon);
void set_chip8_key(uint8_t chip8_key, bool state);
int8_t map_key_to_chip8_key(CGKeyCode keycode);
