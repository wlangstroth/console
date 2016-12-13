// -----------------------------------------------------------------------------
// CONSOLE
//
//
#include "console.h"
#include "oanda.h"


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
    price_map sparkline_prices[],
    int count)
{
    int sparkline_top = GRID_Y_OFFSET + 2 * GRID_EDGE;
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

void
draw_balance(SDL_Renderer *renderer,
         double account_balance,
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

    double bet_fraction = account_balance * 0.02;
    char bet_fraction_string[6];
    snprintf(bet_fraction_string, 6, "%f", bet_fraction);

    draw_text(renderer,
          "2%",
          label_color,
          smaller_font, label_size,
          x + 150, y + 5);
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

Uint32
event_callback(Uint32 interval, void *param)
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
             GRID_Y_OFFSET + GRID_EDGE + 10,
             GRID_EDGE * 15,
             0x66);

    draw_text(renderer,
          "CURRENCIES",
          caption_color,
          smaller_font, 14,
          GRID_X_OFFSET + 33 * GRID_EDGE,
          GRID_Y_OFFSET + GRID_EDGE - 6);

    horizontal_separator(renderer,
             GRID_X_OFFSET + 49 * GRID_EDGE,
             GRID_Y_OFFSET + GRID_EDGE + 10,
             GRID_EDGE * 15,
             0x66);

    draw_text(renderer,
          "CFDs",
          caption_color,
          smaller_font, 14,
          GRID_X_OFFSET + 49 * GRID_EDGE,
          GRID_Y_OFFSET + GRID_EDGE - 6);
}

int
main(int argc, char* argv[])
{
    int result = EXIT_FAILURE;

    Uint32    ticks          = 0;
    Uint32    redraw_delay      = 250;
    Uint32      price_delay       = 5000;
    Uint32      balance_delay     = 60000;
    Uint32    last_price_poll      = 0;
    Uint32    last_balance_poll = 0;

    bool    quit = false;
    SDL_Event    e;

    SDL_Window        *window;
    SDL_Renderer    *renderer;

    price_map    sparkline_prices[10];
    double    account_balance = oanda_balance();

    if (TTF_Init()) {
    printf("TTF_Init: %s\n", TTF_GetError());
    goto bail;
    }

    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER))
    {
    printf("Error initializing SDL: %s\n", SDL_GetError());
    goto bail;
    }

    SDL_AddTimer(redraw_delay, event_callback, (void *)NULL);
    // SDL_AddTimer(price_delay, event_callback, (void *)NULL);
    // SDL_AddTimer(balance_delay, event_callback, (void *)NULL);

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

    oanda_prices(sparkline_prices, 10);

    while (!quit)
    {
    // Paint the black background
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    draw_grid(renderer);

    ticks = SDL_GetTicks();

    // Poll for prices every 5s
    if (ticks - last_price_poll > price_delay)
    {
        oanda_prices(sparkline_prices, 10);
        last_price_poll = ticks;
    }

    // Poll for balance every min
    if (ticks - last_balance_poll > balance_delay)
    {
        last_balance_poll = ticks;
    }

    draw_separators(renderer);
    draw_sparkline_labels(renderer);

    draw_sparklines(renderer, sparkline_prices, 10);

    draw_clock(renderer, 500, 70);

    draw_balance(renderer, account_balance, 1200, 815);

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
