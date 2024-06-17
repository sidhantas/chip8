# Chip 8

This is a relatively small project that emulates the CHIP-8 computer. I built it to get my feet wet in emulation. It runs all CHIP-8 programs/games but it only features a very slow terminal display.
The main process uses a fetch decode execute loop, and sense this CPU has fixed-length instructions it's fairly simple to do all of these in one step. Graphics are handled using the `ncurses` library to reduce
overall project complexity and because the resolution of this computer was so small. Memory was simply implemented as a block of bytes that has unprotected read / write access. Tested on games like Pong, Breakout, and Snake.


## Controls
* Typically the CHIP-8 uses a numpad for controls, but I've mapped them to different keys in the case of no numpad:

```
1 2 3 4
q w e r
a s d f
z x c v
```

## Dependencies

* ncurses

## Build

```
make
```

## Usage

```
./chip8 -i [rom] -f [frequency]
```
