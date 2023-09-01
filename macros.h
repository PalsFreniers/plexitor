#pragma once

// screen macros
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

// frame macros
#define FPS 60

// time macros
#define DELTA_TIME (1.0f/FPS)
#define SPEED 4

// font macros
#define FONT_WIDTH 128
#define FONT_HEIGHT 64
#define FONT_ROWS 7
#define FONT_COLS 18
#define FONT_CHARACTER_WIDTH  (FONT_WIDTH  / FONT_COLS)
#define FONT_CHARACTER_HEIGHT (FONT_HEIGHT / FONT_ROWS)
#define FONT_SCALE 3

// glyph macros
#define GLYPH_BUFFER_CAPACITY 1024

// ASCII macros
#define ASCII_DISPLAY_LOW 32
#define ASCII_DISPLAY_HIGH 127

// conversion macros
#define STR(x) #x

// utility macros
#define TODO(x) assert(false && "TODO : " x)
#define UNUSED(x) ((void)x)
#define UNREACHABLE() assert(false && "UNREACHABLE !!")

// color macro
#define UNHEX(c)  \
    (c) >> (8 * 0) & 0xFF,\
    (c) >> (8 * 1) & 0xFF,\
    (c) >> (8 * 2) & 0xFF,\
    (c) >> (8 * 3) & 0xFF

// log macros
#define SUCCESS_ARG(x, ...) fprintf(stdout, "\x1b[32;1;4m[SUCCESS] : " x "\n\x1b[0;0;0m", __VA_ARGS__)
#define LOG_ARG(x, ...) fprintf(stdout, "\x1b[1;4m[LOG] : " x "\n\x1b[0;0;0m", __VA_ARGS__)
#define WARNING_ARG(x, ...) fprintf(stderr, "\x1b[33;1;4m[WARNING] : " x "\n\x1b[0;0;0m", __VA_ARGS__)
#define ERROR_ARG(x, ...) fprintf(stderr, "\x1b[31;1;4m[ERROR] : " x "\n\x1b[0;0;0m", __VA_ARGS__)
#define DEBUG_ARG(x, ...) fprintf(stdout, "\x1b[34;1;4m[DEBUG] : " x "\n\x1b[0;0;0m", __VA_ARGS__)

#define SUCCESS(x) fprintf(stdout, "\x1b[32;1;4m[SUCCESS] : " x "\n\x1b[0;0;0m")
#define LOG(x) fprintf(stdout, "\x1b[1;4m[LOG] : " x "\n\x1b[0;0;0m")
#define WARNING(x) fprintf(stderr, "\x1b[33;1;4m[WARNING] : " x "\n\x1b[0;0;0m")
#define ERROR(x) fprintf(stderr, "\x1b[31;1;4m[ERROR] : " x "\n\x1b[0;0;0m")
#define DEBUG(x) fprintf(stdout, "\x1b[34;1;4m[DEBUG] : " x "\n\x1b[0;0;0m")
