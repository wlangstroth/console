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

#include "secrets.h"

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

void
draw_grid(SDL_Renderer *renderer)
{
    int i, j;

    int grid_alpha = 0x22;
    int over_alpha = 0x11;

    SDL_SetRenderDrawColor(renderer, 0x77, 0xCC, 0xDD, grid_alpha);
    for (i = GRID_X_OFFSET; i < SCREEN_WIDTH; i += GRID_EDGE)
    {
	for (j = GRID_Y_OFFSET; j < SCREEN_HEIGHT; j += GRID_EDGE)
	{
	    SDL_Rect dot = {i, j, 2, 2};
	    SDL_RenderDrawRect(renderer, &dot);
	}
    }

    // Draw the light grid
    SDL_SetRenderDrawColor(renderer, 0x77, 0xCC, 0xDD, over_alpha);
    for (i = GRID_X_OFFSET; i < SCREEN_WIDTH; i += GRID_EDGE * 2)
    {
	for (j = GRID_Y_OFFSET; j < SCREEN_HEIGHT; j += GRID_EDGE * 2)
	{
	    SDL_Rect dot = {i, j, 2, 2};
	    SDL_RenderDrawRect(renderer, &dot);
	}
    }
}

TTF_Font *
init_font(const char *font_file, int size)
{
    TTF_Font *font = TTF_OpenFont("fonts/Abel-Regular.ttf", size);
    if (!font)
    {
	printf("Font error: %s\n", TTF_GetError());
    }
    return font;
}

void
draw_text(SDL_Renderer *renderer,
	  const char *text,
	  SDL_Color text_color,
	  const char *ttf_file,
	  int size,
	  int x, int y)
{
    SDL_Rect font_rect;
    TTF_Font *font = init_font(ttf_file, size);
    SDL_Surface *text_surface =
	TTF_RenderText_Blended(font, text, text_color);
    SDL_Texture *texture =
	SDL_CreateTextureFromSurface(renderer, text_surface);
    SDL_QueryTexture(texture, NULL, NULL, &font_rect.w, &font_rect.h);

    font_rect.x = x;
    font_rect.y = y;

    SDL_RenderCopy(renderer, texture, NULL, &font_rect);

    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(texture);
    TTF_CloseFont(font);
}

void
draw_clock(SDL_Renderer *renderer,
	   int x,
	   int y)
{
    SDL_Color clock_color = {0xFF,0xFF,0xFF,0xAA};
    SDL_Color label_color = {0x77,0xCC,0xDD,0xAA};

    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = gmtime(&rawtime);

    char *the_time = (char*)malloc(6 * sizeof(char));
    strftime(the_time, 6, "%H:%M", timeinfo);
    draw_text(renderer,
	      "GMT",
	      label_color,
	      "fonts/Abel-Regular.ttf", 18,
	      x - 37,
	      y + 18);
    draw_text(renderer,
	      the_time,
	      clock_color,
	      "fonts/Abel-Regular.ttf", 36,
	      x, y);
    free(the_time);
}

// See https://curl.haxx.se/libcurl/c/getinmemory.html
static size_t
http_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct CurlData *mem = (struct CurlData *)userp;

    mem->data = realloc(mem->data, mem->size + realsize + 1);
    if (mem->data == NULL)
    {
	printf("not enough data (realloc returned NULL)\n");
	return 0;
    }

    memcpy(&(mem->data[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->data[mem->size] = 0;

    return realsize;
}

void
horizontal_separator(SDL_Renderer *renderer, int x, int y, int length, int alpha)
{
    SDL_SetRenderDrawColor(renderer, 0x77, 0xCC, 0xDD, alpha);
    SDL_Rect line = {x, y, length + 1, 2};
    SDL_RenderDrawRect(renderer, &line);

    SDL_SetRenderDrawColor(renderer, 0x77, 0xCC, 0xDD, 0xBB);
    SDL_Rect line_cap = {x, y, 2, 2};
    SDL_RenderDrawRect(renderer, &line_cap);
    line_cap.x = x + length;
    SDL_RenderDrawRect(renderer, &line_cap);
}

void
vertical_separator(SDL_Renderer *renderer, int x, int y, int length, int alpha)
{
    SDL_SetRenderDrawColor(renderer, 0x77, 0xCC, 0xDD, alpha);
    SDL_Rect line = {x, y, 2, length};
    SDL_RenderDrawRect(renderer, &line);

    SDL_SetRenderDrawColor(renderer, 0x77, 0xCC, 0xDD, 0xBB);
    SDL_Rect line_cap = {x, y, 2, 2};
    SDL_RenderDrawRect(renderer, &line_cap);
    line_cap.y = y + length - 1;
    SDL_RenderDrawRect(renderer, &line_cap);
}

void
thick_separator(SDL_Renderer *renderer, int x, int y, int length)
{
    SDL_SetRenderDrawColor(renderer, 0x77, 0xCC, 0xDD, 0xBB);
    SDL_Rect left = {x, y - 5, 2, length + 10};
    SDL_Rect right = {x + 2, y - 5, 2, length + 10};
    SDL_RenderDrawRect(renderer, &left);
    SDL_RenderDrawRect(renderer, &right);
}

void
draw_separators(SDL_Renderer *renderer)
{
    // Page top decoration
    horizontal_separator(renderer,
		       GRID_X_OFFSET,
		       GRID_Y_OFFSET,
		       30 * GRID_EDGE,
		       0x77);
    horizontal_separator(renderer,
		       GRID_X_OFFSET,
		       GRID_Y_OFFSET + 6,
		       30 * GRID_EDGE,
		       0x44);
    horizontal_separator(renderer,
		       698,
		       GRID_Y_OFFSET,
		       33 * GRID_EDGE,
		       0x77);
    horizontal_separator(renderer,
		       698,
		       GRID_Y_OFFSET + 6,
		       33 * GRID_EDGE,
		       0x44);
    // Page bottom decoration
    horizontal_separator(renderer,
		       GRID_X_OFFSET,
		       GRID_Y_OFFSET + GRID_EDGE * 39,
		       30 * GRID_EDGE,
		       0x44);
    horizontal_separator(renderer,
		       GRID_X_OFFSET,
		       GRID_Y_OFFSET + 6 + GRID_EDGE * 39,
		       30 * GRID_EDGE,
		       0x44);
    horizontal_separator(renderer,
		       698,
		       GRID_Y_OFFSET + GRID_EDGE * 39,
		       33 * GRID_EDGE,
		       0x44);
    horizontal_separator(renderer,
		       698,
		       GRID_Y_OFFSET + 6 + GRID_EDGE * 39,
		       33 * GRID_EDGE,
		       0x44);

    // Clock baseline
    horizontal_separator(renderer,
		       GRID_X_OFFSET + GRID_EDGE * 57,
		       GRID_Y_OFFSET + GRID_EDGE * 38,
		       6 * GRID_EDGE,
		       0x44);
    vertical_separator(renderer,
		       GRID_X_OFFSET + GRID_EDGE * 57,
		       GRID_Y_OFFSET + GRID_EDGE * 38,
		       GRID_EDGE / 2,
		       0x44);
    vertical_separator(renderer,
		       GRID_X_OFFSET + GRID_EDGE * 63,
		       GRID_Y_OFFSET + GRID_EDGE * 38 - 11,
		       GRID_EDGE / 2,
		       0x44);


    horizontal_separator(renderer,
			 GRID_X_OFFSET + GRID_EDGE * 31,
			 GRID_Y_OFFSET + GRID_EDGE * 33 - 6,
			 33 * GRID_EDGE,
			 0x44);
    horizontal_separator(renderer,
			 GRID_X_OFFSET + GRID_EDGE * 31,
			 GRID_Y_OFFSET + GRID_EDGE * 33,
			 33 * GRID_EDGE,
			 0x44);


}

void
draw_sparkline(SDL_Renderer *renderer,
	       const char *label,
	       double sparkline_price,
   	       int x,
	       int y)
{
    SDL_SetRenderDrawColor(renderer, 0x77, 0xCC, 0xDD, 0x66);
    int axis_height = 6 * GRID_EDGE;
    int axis_width = 15 * GRID_EDGE;
    SDL_Rect left_axis = {x, y, 2, axis_height};
    SDL_Rect bottom_axis = {x, y + axis_height, axis_width + 2, 2};
    SDL_Rect right_axis = {x + axis_width, y, 2, axis_height};

    SDL_Rect axes[3] = {left_axis, bottom_axis, right_axis};

    SDL_RenderDrawRects(renderer, axes, 3);

    SDL_Color instrument_color = {0x77,0xCC,0xDD,0xFF};
    SDL_Color price_color = {0xFF, 0xFF, 0xFF,0xFF};
    int title_size = 30;
    int price_size = 20;

    draw_text(renderer,
	      label,
	      instrument_color,
	      "fonts/Abel-Regular.ttf",
	      title_size,
	      x + 5, y + GRID_EDGE * 6 - title_size - 4);

    int number_length = 8;
    char price_label[8];

    snprintf(price_label, number_length, "%f", sparkline_price);
    draw_text(renderer,
	      price_label,
	      price_color,
	      "fonts/Abel-Regular.ttf",
	      price_size,
	      x + 262, y + GRID_EDGE * 6 - price_size - 5);
}

void
draw_sparklines(SDL_Renderer *renderer, struct price_map sparkline_prices[])
{
    int sparkline_top = GRID_Y_OFFSET + 2 * GRID_EDGE;
    int sparkline_left = GRID_X_OFFSET + 32 * GRID_EDGE;
    int sparkline_right = GRID_X_OFFSET + 48 * GRID_EDGE;
    int offset = 7;
    int left_edge = sparkline_left;
    int multiplier = 0;

    for (int i = 0; i < 8; i++) {

	if (i < 4) {
	    left_edge = sparkline_left;
	    multiplier = i;
	}
	else
	{
	    left_edge = sparkline_right;
	    multiplier = i - 4;

	}
	draw_sparkline(renderer,
		       sparkline_prices[i].key,
		       sparkline_prices[i].value,
		       left_edge,
		       sparkline_top + multiplier * offset * GRID_EDGE);
    }
        thick_separator(renderer,
		    GRID_X_OFFSET + GRID_EDGE * 31 + 1,
		    GRID_Y_OFFSET + GRID_EDGE * 2,
		    GRID_EDGE * 6);
    thick_separator(renderer,
		    GRID_X_OFFSET + GRID_EDGE * 31 + 1,
		    GRID_Y_OFFSET + GRID_EDGE * 9,
		    GRID_EDGE * 6);
    thick_separator(renderer,
		    GRID_X_OFFSET + GRID_EDGE * 31 + 1,
		    GRID_Y_OFFSET + GRID_EDGE * 16,
		    GRID_EDGE * 6);
    thick_separator(renderer,
		    GRID_X_OFFSET + GRID_EDGE * 31 + 1,
		    GRID_Y_OFFSET + GRID_EDGE * 23,
		    GRID_EDGE * 6);

    thick_separator(renderer,
		    GRID_X_OFFSET + GRID_EDGE * 64 - 3,
		    GRID_Y_OFFSET + GRID_EDGE * 2,
		    GRID_EDGE * 6);
    thick_separator(renderer,
		    GRID_X_OFFSET + GRID_EDGE * 64 - 3,
		    GRID_Y_OFFSET + GRID_EDGE * 9,
		    GRID_EDGE * 6);
    thick_separator(renderer,
		    GRID_X_OFFSET + GRID_EDGE * 64 - 3,
		    GRID_Y_OFFSET + GRID_EDGE * 16,
		    GRID_EDGE * 6);
    thick_separator(renderer,
		    GRID_X_OFFSET + GRID_EDGE * 64 - 3,
		    GRID_Y_OFFSET + GRID_EDGE * 23,
		    GRID_EDGE * 6);
}

struct price_map sparkline_prices[8];

void
pull_prices()
{
    CURL *curl_handle;
    char auth_header[100];
    if (snprintf(auth_header, 100, "Authorization: Bearer %s", access_token) >= 100)
    {
	printf("Auth header overflow.");
	exit(EXIT_FAILURE);
    }

    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();

    CURLcode response_code;
    struct curl_slist *slist = NULL;
    struct CurlData chunk;

    chunk.data = malloc(1);
    chunk.size = 0;

    char *url =
	"https://api-fxtrade.oanda.com/v1/prices?instruments=EUR_USD%2CGBP_USD%2CUSD_JPY%2CUSD_CAD%2CUSD_CHF%2CSPX500_USD%2CXCU_USD%2CUSB30Y_USD";
    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, http_callback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "console/1.0");
    slist = curl_slist_append(slist, auth_header);

    curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, slist);
    response_code = curl_easy_perform(curl_handle);

    if (response_code)
    {
	fprintf(stderr, "curl_easy_perform() failed: %s\n",
		curl_easy_strerror(response_code));
    }
    else
    {
	// parse_chunk_data(chunk.data);
	struct json_object *parse_result = json_tokener_parse(chunk.data);
	json_object_object_foreach(parse_result, key, val) {
	    json_object *arr = NULL;
	    json_object *price_object = NULL;
	    json_object *instrument = NULL;
	    json_object *price = NULL;
	    json_object_object_get_ex(parse_result, key, &arr);
	    int arrlen = json_object_array_length(arr);

	    for (int i = 0; i < arrlen; i++) {
		price_object = json_object_array_get_idx(arr, i);
		json_object_object_get_ex(price_object, "instrument", &instrument);

		json_object_object_get_ex(price_object, "bid", &price);
		sparkline_prices[i].key = json_object_get_string(instrument);
		sparkline_prices[i].value = json_object_get_double(price);
	    }
	}
	// fprintf(stdout, "%s\n", chunk.data);
    }

    curl_easy_cleanup(curl_handle);
    curl_slist_free_all(slist);
    free(chunk.data);
    curl_global_cleanup();
}

double account_balance = 0.0;
double bet_fraction = 0.0;

void
pull_balance()
{
    CURL *curl_handle;
    char auth_header[100];
    if (snprintf(auth_header, 100, "Authorization: Bearer %s", access_token) >= 100)
    {
	printf("Auth header overflow.");
	exit(EXIT_FAILURE);
    }

    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();

    CURLcode response_code;
    struct curl_slist *slist = NULL;
    struct CurlData chunk;

    chunk.data = malloc(1);
    chunk.size = 0;

    char url[51];
    if (snprintf(url, 51, "https://api-fxtrade.oanda.com/v1/accounts/%s", account_id) >= 51)
    {
	fprintf(stdout, "URL overflow.");
	exit(EXIT_FAILURE);
    }
    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, http_callback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "console/1.0");
    slist = curl_slist_append(slist, auth_header);

    curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, slist);
    response_code = curl_easy_perform(curl_handle);

    if (response_code)
    {
	fprintf(stderr, "curl_easy_perform() failed: %s\n",
		curl_easy_strerror(response_code));
    }
    else
    {
	json_object *balance_obj = NULL;
	struct json_object *parse_result = json_tokener_parse(chunk.data);

	json_object_object_get_ex(parse_result, "balance", &balance_obj);
	account_balance = json_object_get_double(balance_obj);
	bet_fraction = account_balance * 0.02;
    }

}

void
draw_balance(SDL_Renderer *renderer,
	     int x,
	     int y)
{
    SDL_Color balance_color = {0xFF,0xFF,0xFF,0xAA};
    char balance_string[8];
    snprintf(balance_string, 8, "%f", account_balance);

    draw_text(renderer,
	      balance_string,
	      balance_color,
	      "fonts/Abel-Regular.ttf", 36,
	      x, y);

    char bet_fraction_string[6];
    snprintf(bet_fraction_string, 6, "%f", bet_fraction);
    draw_text(renderer,
	      bet_fraction_string,
	      balance_color,
	      "fonts/Abel-Regular.ttf", 28,
	      x + 50, y + 50);
}

int
main(int argc, char* argv[])
{
    int result = EXIT_FAILURE;

    Uint32 ticks = 0;
    Uint32 last_curl = 0;

    if (TTF_Init()) {
	printf("TTF_Init: %s\n", TTF_GetError());
	goto bail;
    }

    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER))
    {
	printf("Error initializing SDL: %s\n", SDL_GetError());
	goto bail;
    }

    SDL_Window *window;
    SDL_Renderer *renderer;

    SDL_CreateWindowAndRenderer(0, 0,
				SDL_WINDOW_FULLSCREEN_DESKTOP,
				&window,
				&renderer);

    if (!window)
    {
	printf("Error creating window: %s\n", SDL_GetError());
	goto bail;
    }

    if (!renderer)
    {
	printf("Error creating renderer: %s\n", SDL_GetError());
	goto bail;
    }

    bool quit = false;
    SDL_Event e;

    pull_prices();
    pull_balance();

    while (!quit)
    {
	// Paint the black background
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	draw_grid(renderer);

	ticks = SDL_GetTicks();

	if (ticks - last_curl > 5000)
	{
	    pull_prices();
	    last_curl = SDL_GetTicks();
	}

	draw_separators(renderer);

	draw_sparklines(renderer, sparkline_prices);

	draw_clock(renderer, 1313, 805);

	draw_balance(renderer, 1000, 700);

	SDL_RenderPresent(renderer);

	// Handle events on queue
	while (SDL_PollEvent(&e)) {
	    if (e.type == SDL_QUIT || e.type == SDL_WINDOWEVENT_CLOSE)
	    {
		quit = true;
	    }
	}
	SDL_Delay(80);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    result = EXIT_SUCCESS;

bail:
    SDL_Quit();
    TTF_Quit();

    return result;
}