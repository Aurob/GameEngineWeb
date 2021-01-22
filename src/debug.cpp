#include <emscripten.h>
#include <cstdlib>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <algorithm>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

struct context
{
    SDL_Renderer *renderer;
    int iteration;
};

const unsigned int WIDTH = 1000;
const unsigned int HEIGHT = 1000;
unsigned int tilesize = 64;

SDL_Rect temp_rect;
temp_rect.w = tilesize;
temp_rect.h = tilesize;

void mainloop(void *arg)
{   
    context *ctx = static_cast<context*>(arg);
    SDL_Renderer *renderer = ctx->renderer;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255 );
    SDL_RenderClear(renderer);

    for(int x = 0; x < WIDTH%tilesize; x++){
        for(int y = 0; y < HEIGHT%tilesize; y++){
            temp_rect.x = x*tilesize;
            temp_rect.y = y*tilesize;
            int g = rand() % 255;
            SDL_SetRenderDrawColor(renderer, g, g, g, 255 );

            SDL_RenderFillRect(renderer, &temp_rect);
        }
    }
    
    

    //finally draw everything to the screen
    SDL_RenderPresent(renderer);
    ctx->iteration++;
}
int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer);

    context ctx;
    ctx.renderer = renderer;
    ctx.iteration = 0;

    const int simulate_infinite_loop = 1; // call the function repeatedly
    const int fps = -1; // call the function as fast as the browser wants to render (typically 60fps)
    //emscripten_run_script("var ws = new WebSocket('wss://robauis.me/ws'); ws.onmessage = (e)=>{console.log(e.data);}");

    emscripten_set_main_loop_arg(mainloop, &ctx, fps, simulate_infinite_loop);
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
