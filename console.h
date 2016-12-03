#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include <curl/curl.h>
#include <json-c/json.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2_ttf/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#define SCREEN_WIDTH 1440
#define SCREEN_HEIGHT 900
#define GRID_X_OFFSET 16
#define GRID_Y_OFFSET 8
#define GRID_EDGE 22

struct price_map {
    const char *key;
    double value;
};

struct CurlData {
    char *data;
    size_t size;
};

#endif
