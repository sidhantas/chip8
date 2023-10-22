#include <CoreGraphics/CoreGraphics.h>
#include <ApplicationServices/ApplicationServices.h>
#include <stdint.h>
#include "IO.h"
#include "chip8.h"

#ifdef __APPLE__
void *enable_keyboard_raw_mode() {
    CGEventMask eventMask = CGEventMaskBit(kCGEventKeyDown) | CGEventMaskBit(kCGEventFlagsChanged) | CGEventMaskBit(kCGEventKeyUp);
    CFMachPortRef eventTap = CGEventTapCreate(
        kCGSessionEventTap, kCGHeadInsertEventTap, 0, eventMask, CGEventCallback, NULL
    );

    if (!eventTap) {
        fprintf(stderr, "ERROR: Unable to create event tap.\n");
        exit(1);
    }

    CFRunLoopSourceRef runLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, eventTap, 0);
    CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopCommonModes);
    CGEventTapEnable(eventTap, true);
    fflush(stdout);
    CFRunLoopRun();
    return NULL;
}

CGEventRef CGEventCallback(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void *refcon) {
    if (type != kCGEventKeyDown && type != kCGEventFlagsChanged && type != kCGEventKeyUp) {
        return event;
    }

    CGEventFlags flags = CGEventGetFlags(event);

    CGKeyCode keycode = (CGKeyCode) CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);
    int8_t mapped_key = map_key_to_chip8_key(keycode); 
    if (type == kCGEventKeyDown && mapped_key >= 0) {
        set_chip8_key(mapped_key, true);
    } else if (type == kCGEventKeyUp && mapped_key >= 0) {
        set_chip8_key(mapped_key, false);
        most_recent_key = mapped_key;
    } 
    fflush(stdout);
    return event;
}

#endif

int8_t map_key_to_chip8_key(uint16_t keycode) {
    switch (keycode) {
        case 18: return 0x1; // 1
        case 19: return 0x2; // 2
        case 20: return 0x3; // 3
        case 21: return 0xC; // 4
        case 12: return 0x4; // q
        case 13: return 0x5; // w
        case 14: return 0x6; // e
        case 15: return 0xD; // r
        case 0:  return 0x7; // a
        case 1:  return 0x8; // s
        case 2:  return 0x9; // d
        case 3:  return 0xE; // f
        case 6:  return 0xA; // z
        case 7:  return 0x0; // x
        case 8:  return 0xB; // c
        case 9:  return 0xF; // v
        default: return -1;
    }
}

void set_chip8_key(uint8_t chip8_key, bool state) {
    if (chip8_key < 0 || chip8_key > KEYPAD_SIZE) {
        return;
    }
    kp[chip8_key] = state; 
}
