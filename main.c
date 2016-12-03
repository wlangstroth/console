// -----------------------------------------------------------------------------
// CONSOLE
//
#include "console.h"
#include "secrets.h"

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

char *default_font = "fonts/Abel-Regular.ttf";
char *smaller_font = "fonts/Lato-Bold.ttf";

TTF_Font *
init_font(const char *font_file, int size)
{
    TTF_Font *font = TTF_OpenFont(font_file, size);
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
	      smaller_font, 14,
	      x - 37,
	      y + 22);
    draw_text(renderer,
	      the_time,
	      clock_color,
	      default_font, 36,
	      x, y);
    free(the_time);

    // horizontal_separator(renderer, )
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
draw_separators(SDL_Renderer *renderer)
{
    // Page top decoration
    horizontal_separator(renderer,
			 GRID_X_OFFSET,
			 GRID_Y_OFFSET,
			 32 * GRID_EDGE,
			 0x77);
    horizontal_separator(renderer,
			 GRID_X_OFFSET,
			 GRID_Y_OFFSET + 6,
			 32 * GRID_EDGE,
			 0x44);
    horizontal_separator(renderer,
			 GRID_X_OFFSET + GRID_EDGE * 33,
			 GRID_Y_OFFSET,
			 31 * GRID_EDGE,
			 0x77);
    horizontal_separator(renderer,
			 GRID_X_OFFSET + GRID_EDGE * 33,
			 GRID_Y_OFFSET + 6,
			 31 * GRID_EDGE,
			 0x44);

    // Page bottom decoration
    horizontal_separator(renderer,
			 GRID_X_OFFSET,
			 GRID_Y_OFFSET + GRID_EDGE * 40,
			 32 * GRID_EDGE,
			 0x44);
    horizontal_separator(renderer,
			 GRID_X_OFFSET,
			 GRID_Y_OFFSET + 6 + GRID_EDGE * 40,
			 32 * GRID_EDGE,
			 0x44);

    horizontal_separator(renderer,
			 GRID_X_OFFSET + GRID_EDGE * 33,
			 GRID_Y_OFFSET + GRID_EDGE * 40,
			 31 * GRID_EDGE,
			 0x44);
    horizontal_separator(renderer,
			 GRID_X_OFFSET + GRID_EDGE * 33,
			 GRID_Y_OFFSET + 6 + GRID_EDGE * 40,
			 31 * GRID_EDGE,
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

    roundedRectangleRGBA(renderer,
			 x, y,
			 x + axis_width, y + axis_height,
			 2,
			 0x77, 0xCC, 0xDD, 0xAA);

    SDL_Color instrument_color = {0x77,0xCC,0xDD,0xFF};
    SDL_Color price_color = {0xFF, 0xFF, 0xFF,0xFF};
    int title_size = 30;
    int price_size = 20;

    draw_text(renderer,
	      label,
	      instrument_color,
	      default_font,
	      title_size,
	      x + 5, y + GRID_EDGE * 6 - title_size - 4);

    int number_length = 8;
    char price_label[8];

    snprintf(price_label, number_length, "%f", sparkline_price);
    draw_text(renderer,
	      price_label,
	      price_color,
	      default_font,
	      price_size,
	      x + 262, y + GRID_EDGE * 6 - price_size - 5);
}

void
draw_sparklines(
    SDL_Renderer *renderer,
    struct price_map sparkline_prices[],
    int count)
{
    int sparkline_top = GRID_Y_OFFSET + 3 * GRID_EDGE;
    int sparkline_left = GRID_X_OFFSET + 33 * GRID_EDGE;
    int sparkline_right = GRID_X_OFFSET + 49 * GRID_EDGE;
    int offset = 7;
    int left_edge = sparkline_left;
    int multiplier = 0;
    const char *label = NULL;

    for (int i = 0; i < count; i++) {
	if (i < count / 2) {
	    left_edge = sparkline_left;
	    multiplier = i;
	}
	else
	{
	    left_edge = sparkline_right;
	    multiplier = i - count / 2;
	}

	label = sparkline_prices[i].key;
	if (!strcmp(label, "USB30Y_USD"))
	{
	    label = "T - BOND";
	}
	if (!strcmp(label, "SPX500_USD"))
	{
	    label = "SPX 500";
	}
	if (!strcmp(label, "XCU_USD"))
	{
	    label = "COPPER";
	}
	if (!strcmp(label, "EUR_USD"))
	{
	    label = "EUR USD";
	}
	if (!strcmp(label, "GBP_USD"))
	{
	    label = "GBP USD";
	}
	if (!strcmp(label, "USD_CAD"))
	{
	    label = "USD CAD";
	}
	if (!strcmp(label, "USD_JPY"))
	{
	    label = "USD JPY";
	}
	if (!strcmp(label, "USD_CHF"))
	{
	    label = "USD CHF";
	}
	if (!strcmp(label, "SOYBN_USD"))
	{
	    label = "SOYBEANS";
	}
	if (!strcmp(label, "NATGAS_USD"))
	{
	    label = "NATURAL GAS";
	}

	draw_sparkline(renderer,
		       label,
		       sparkline_prices[i].value,
		       left_edge,
		       sparkline_top + multiplier * offset * GRID_EDGE);
    }
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

struct json_object *
curl(const char *url)
{
    CURL *curl_handle;
    char auth_header[100];
    struct json_object *result = NULL;

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
	exit(EXIT_FAILURE);
    }
    else
    {
	result = json_tokener_parse(chunk.data);
    }

    curl_easy_cleanup(curl_handle);
    curl_slist_free_all(slist);
    free(chunk.data);
    curl_global_cleanup();

    return result;
}



void
pull_prices(struct price_map sparkline_prices[], int count)
{
    char *url =
	"https://api-fxtrade.oanda.com/v1/prices?instruments=EUR_USD%2CGBP_USD%2CUSD_JPY%2CUSD_CAD%2CUSD_CHF%2CSPX500_USD%2CXCU_USD%2CUSB30Y_USD%2CSOYBN_USD%2CNATGAS_USD";
    struct json_object *parse_result = curl(url);
    if (parse_result) {
	json_object_object_foreach(parse_result, key, val) {
	    json_object *arr = NULL;
	    json_object *price_object = NULL;
	    json_object *instrument = NULL;
	    json_object *price = NULL;
	    json_object_object_get_ex(parse_result, key, &arr);
	    // int arrlen = json_object_array_length(arr);

	    for (int i = 0; i < count; i++) {
		price_object = json_object_array_get_idx(arr, i);
		json_object_object_get_ex(price_object, "instrument", &instrument);

		json_object_object_get_ex(price_object, "bid", &price);
		sparkline_prices[i].key = json_object_get_string(instrument);
		sparkline_prices[i].value = json_object_get_double(price);
	    }
	}
    }
    else
    {
	for (int i = 0; i < count; i++) {
	    sparkline_prices[i].key = "INSTRUMENT";
	    sparkline_prices[i].value = 0;
	}
    }
}

double account_balance = 0.0;
double bet_fraction = 0.0;

void
pull_balance()
{
    char url[51];
    if (snprintf(url, 51, "https://api-fxtrade.oanda.com/v1/accounts/%s", account_id) >= 51)
    {
	fprintf(stdout, "URL overflow.");
	exit(EXIT_FAILURE);
    }

    json_object *balance_obj = NULL;
    struct json_object *parse_result = curl(url);

    json_object_object_get_ex(parse_result, "balance", &balance_obj);
    account_balance = json_object_get_double(balance_obj);
    bet_fraction = account_balance * 0.02;
}

void
draw_balance(SDL_Renderer *renderer,
	     int x,
	     int y)
{
    SDL_Color balance_color = {0xFF,0xFF,0xFF,0xAA};
    SDL_Color label_color = {0x77,0xCC,0xDD,0xAA};
    char balance_string[8];
    snprintf(balance_string, 8, "%f", account_balance);
    int label_size = 14;
    draw_text(renderer,
	      "BALANCE",
	      label_color,
	      smaller_font, label_size,
	      x, y);
    draw_text(renderer,
	      balance_string,
	      balance_color,
	      default_font, 36,
	      x, y + 12);

    char bet_fraction_string[6];
    snprintf(bet_fraction_string, 6, "%f", bet_fraction);

    draw_text(renderer,
	      "2%",
	      label_color,
	      smaller_font, label_size,
	      x + 150, y);
    draw_text(renderer,
	      bet_fraction_string,
	      balance_color,
	      default_font, 28,
	      x + 150, y + 20);
}

void
draw_candle(SDL_Renderer *renderer,
	    int open,
	    int high,
	    int low,
	    int closep,
	    int x, int y)
{
    SDL_SetRenderDrawColor(renderer, 0xFF,0xFF,0xFF,0xBB);
    SDL_RenderDrawLine(renderer, x + 1, y, x + 1, y + 4);

    SDL_SetRenderDrawColor(renderer, 0x00,0xFF,0x00,0xBB);
    SDL_Rect candle_body = {x, y + 5, 3, 15};
    SDL_RenderFillRect(renderer, &candle_body);

    SDL_SetRenderDrawColor(renderer, 0xFF,0xFF,0xFF,0xBB);
    SDL_RenderDrawLine(renderer, x + 1, y + 20, x + 1, y + 25);
}

void
draw_action_button(SDL_Renderer *renderer)
{
    roundedRectangleRGBA(renderer,
			 50, 600,
			 250, 650,
			 10,
			 0x77, 0xCC, 0xDD, 0xAA);
}

void
draw_main_panel(SDL_Renderer *renderer)
{
    SDL_Color caption_color = {0x77,0xCC,0xDD,0xAA};
    SDL_Color label_color = {0xCC,0xCC,0xCC,0xFF};
    draw_text(renderer,
	      "INSTRUMENT",
	      caption_color,
	      smaller_font, 14,
	      GRID_X_OFFSET, 32);
    horizontal_separator(renderer,
			 GRID_X_OFFSET, 50,
			 200,
			 0x66);

    draw_text(renderer,
	      "EUR USD",
	      label_color,
	      default_font, 48,
	      30, 47);

    horizontal_separator(renderer,
			 GRID_X_OFFSET, 105,
			 250,
			 0x66);
    vertical_separator(renderer,
		       GRID_X_OFFSET, 50,
		       20,
		       0x66);
    vertical_separator(renderer,
		       GRID_X_OFFSET, 85,
		       20,
		       0x66);


    SDL_SetRenderDrawColor(renderer, 0x77, 0xCC, 0xDD, 0xAA);
    int frame_width = GRID_EDGE * 32;
    int frame_height = GRID_EDGE * 20;
    int top_left_y = GRID_X_OFFSET + GRID_EDGE * 5;

    SDL_Rect frame_left = {
	GRID_X_OFFSET,
        top_left_y,
	2,
	frame_height
    };
    SDL_Rect frame_right = {
	GRID_X_OFFSET + frame_width,
	top_left_y,
	2,
	frame_height
    };

    SDL_Rect frame_top = {
	GRID_X_OFFSET + 1,
	top_left_y - 1,
	frame_width,
	2
    };
    SDL_Rect frame_bottom = {
	GRID_X_OFFSET + 1,
	top_left_y + frame_height - 1,
	frame_width,
	2
    };

    SDL_Rect frame_rects[4] = {
	frame_left,
	frame_right,
	frame_top,
	frame_bottom
    };

    SDL_RenderFillRects(renderer, frame_rects, 4);
}

Uint32 my_callbackfunc(Uint32 interval, void *param)
{
    SDL_Event event;
    SDL_UserEvent userevent;

    /* In this example, our callback pushes an SDL_USEREVENT event
    into the queue, and causes our callback to be called again at the
    same interval: */

    userevent.type = SDL_USEREVENT;
    userevent.code = 0;
    userevent.data1 = NULL;
    userevent.data2 = NULL;

    event.type = SDL_USEREVENT;
    event.user = userevent;

    SDL_PushEvent(&event);

    return(interval);
}

void
draw_sparkline_labels(SDL_Renderer *renderer)
{
    SDL_Color caption_color = {0x77,0xCC,0xDD,0xAA};
    horizontal_separator(renderer,
			 GRID_X_OFFSET + 33 * GRID_EDGE,
			 GRID_Y_OFFSET + 2 * GRID_EDGE + 10,
			 GRID_EDGE * 15,
			 0x66);

    draw_text(renderer,
	      "CURRENCIES",
	      caption_color,
	      smaller_font, 14,
	      GRID_X_OFFSET + 33 * GRID_EDGE,
	      GRID_Y_OFFSET + 2 * GRID_EDGE - 10);

    horizontal_separator(renderer,
			 GRID_X_OFFSET + 49 * GRID_EDGE,
			 GRID_Y_OFFSET + 2 * GRID_EDGE + 10,
			 GRID_EDGE * 15,
			 0x66);

    draw_text(renderer,
	      "CFDs",
	      caption_color,
	      smaller_font, 14,
	      GRID_X_OFFSET + 49 * GRID_EDGE,
	      GRID_Y_OFFSET + 2 * GRID_EDGE - 10);

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
    Uint32 delay = 250;
    // SDL_TimerID timer_id = SDL_AddTimer(delay, my_callbackfunc, NULL);
    SDL_AddTimer(delay, my_callbackfunc, NULL);

    SDL_Window *window;
    SDL_Renderer *renderer;

    window = SDL_CreateWindow("console",
			      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			      0, 0,
			      SDL_WINDOW_FULLSCREEN_DESKTOP);
    renderer = SDL_CreateRenderer(window, -1,
				  SDL_RENDERER_ACCELERATED |
				  SDL_RENDERER_PRESENTVSYNC);

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

    struct price_map sparkline_prices[10];
    pull_prices(sparkline_prices, 10);

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
	    pull_prices(sparkline_prices, 10);
	    last_curl = SDL_GetTicks();
	}

	draw_separators(renderer);
	draw_sparkline_labels(renderer);

	draw_sparklines(renderer, sparkline_prices, 10);

	draw_clock(renderer, 500, 70);

	draw_balance(renderer, 450, 805);

	draw_candle(renderer, 0, 0, 0, 0, 400, 400);
	draw_candle(renderer, 0, 0, 0, 0, 405, 385);
	draw_candle(renderer, 0, 0, 0, 0, 410, 370);

	draw_main_panel(renderer);

	int mouse_x, mouse_y;
	SDL_GetMouseState(&mouse_x, &mouse_y);
	SDL_Point mouse_position = {mouse_x, mouse_y};
	SDL_Rect button_area = {50, 200, 200, 50};

	if (SDL_PointInRect(&mouse_position, &button_area))
	{
	    roundedBoxRGBA(renderer,
			   250, 200,
			   50, 250,
			   10,
			   0x77, 0xCC, 0xDD, 0xAA);
	}
	else
	{
	    draw_action_button(renderer);
	}

	SDL_RenderPresent(renderer);

	if (SDL_WaitEvent(&e))
	{
	    switch (e.type)
	    {
	    case SDL_QUIT:
		quit = true;
	    }

	}
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    result = EXIT_SUCCESS;

bail:
    SDL_Quit();
    TTF_Quit();

    return result;
}
