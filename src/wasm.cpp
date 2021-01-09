#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <emscripten.h>
#include <cstdlib>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <algorithm>
#include "FastNoise.h"
#include "Game.h"



struct context
{
    SDL_Renderer *renderer;
    int iteration;
};

const int MAX_char = 8;

//
Game game{};

std::vector<Texture> icons;
std::vector<Texture> charicons;
std::vector<Texture> tileicons;
std::vector<Texture> popupicons;

SDL_Texture *img = NULL;
int w, h;
int biometex;
Position uchunk;
Position temp_click;


// Begin JS/C bridges
extern "C" {
    int ecount(){
        return game.user.owned_entities.size();
    }
    const char* get_info(int type){
        const char* retval;
        FastNoise noise;
        switch(type){
            case 0:
                retval = (std::to_string(game.user.mouse["x"]) + ", " + std::to_string(game.user.mouse["y"])).c_str();
                break;

            case 1:
                retval = (std::to_string(static_cast<int>(game.user.globalx)) + ", " + std::to_string(static_cast<int>(game.user.globaly))).c_str();
                break;

            case 2:
                retval = (std::to_string(game.user.mouse_chunk[0]) + ", " + std::to_string(game.user.mouse_chunk[1])).c_str();
                break;
            case 3:
                retval = std::to_string(noise.GetPerlin(game.user.chunk[0], game.user.chunk[1])).c_str();
                break;
            case 4:
                retval = game.game.mouse_entity.ID.c_str();
                break;
            case 5:
                retval = std::to_string(game.user.items["fish"]).c_str();
                break;
            case 6:
                retval = std::to_string(game.user.items["wood"]).c_str();
                break;
            case 7:
                retval = std::to_string(game.user.items["stone"]).c_str();
                break;
            default:
                retval = "No type specified";
                break;
        }
        return retval;
    }
}

int SDLCALL EventHandler(void *userdata, SDL_Event *event) {
    int xchunk, ychunk;
    std::string chunk_key;
    Position p;
    temp_click = {};
    switch(event->type) {
        case SDL_MOUSEMOTION:
            game.user.mouse["x"] = event->motion.x;
            game.user.mouse["y"] = event->motion.y;

            break;

        case SDL_MOUSEWHEEL:
            if(event->wheel.y < 0 && game.game.current_chunk_size > 0) game.game.current_chunk_size--;
            if(event->wheel.y > 0 && game.game.current_chunk_size < 5) game.game.current_chunk_size++;
            
            game.game.chunk_size = game.game.chunk_sizes[game.game.current_chunk_size];

            game.game.size = floor(static_cast<float>(game.game.chunk_size) / 2);

            break;

        case SDL_MOUSEBUTTONDOWN:
            game.user.mouse_down = true;
            
            break;   

        case SDL_MOUSEBUTTONUP:
            game.user.mouse_down = false;
            break;

        case SDL_KEYUP:

            switch (event->key.keysym.sym) {
                case SDLK_d: game.user.keyState[1] = false; break;
                case SDLK_a: game.user.keyState[2] = false; break;
                case SDLK_s: game.user.keyState[3] = false; break;
                case SDLK_w: game.user.keyState[4] = false; break;
                case SDLK_LSHIFT: game.user.keyState[5] = false; break;
                default: break;
            }
            break;

        case SDL_KEYDOWN:
            switch (event->key.keysym.sym) {
                case SDLK_d: game.user.keyState[1] = true; break;
                case SDLK_a: game.user.keyState[2] = true; break;
                case SDLK_s: game.user.keyState[3] = true; break;
                case SDLK_w: game.user.keyState[4] = true; break;
                case SDLK_LSHIFT: game.user.keyState[5] = true; break;
                default: break;
            }
            break;
    }

    return -1;
}

EM_JS(int, get_icon_index, (), {
    return getIcon();
});

EM_JS(void, talk, (int type), {

    switch(type){
        case 0:
            i = Math.floor(Math.random() * 100);

            if(i < 80)
                alert("Knock Knock \nNo Answer...");
            else if(i < 85)
                alert("Go away! \n*Stomp Stomp*...");
            else if(i < 90)
                alert("I have a cold, I can't come out!");
            else if(i < 98)
                alert("Something interesting should happen here");
            break;

        case 1:
            alert("You slapped the tree");
            break;
        case 2:
            alert("Can I help you?");
            break;
        default:
            break;
    }
    
});

//If an alert is made, all events need to be cancelled
void send_alert(int type){
    for(auto& e : game.user.keyState){
        game.user.keyState[e.first] = false;
    }
    game.user.mouse_down = false;
    talk(type);
}

float n;
int ei;
float texheight;
int t;
bool skip;    
std::hash<std::string> hasher;
void mainloop(void *arg)
{   
    SDL_Event event;
    //Handle events
    while (SDL_PollEvent(&event)) {
        EventHandler(0, &event);
    }

    context *ctx = static_cast<context*>(arg);
    SDL_Renderer *renderer = ctx->renderer;

    

    
    //finally draw everything to the screen
    SDL_RenderPresent(renderer);
    ctx->iteration++;
}



int main(int argc, char *argv[])
{
    //seed generator
    srand(time(NULL));
    game.game.noise.SetSeed(rand() % 10000);
    
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_CreateWindowAndRenderer(game.game.width, game.game.height, 0, &window, &renderer);

    context ctx;
    ctx.renderer = renderer;
    ctx.iteration = 0;

    const int simulate_infinite_loop = 1; // call the function repeatedly
    const int fps = -1; // call the function as fast as the browser wants to render (typically 60fps)
    //emscripten_run_script("var ws = new WebSocket('wss://robauis.me/ws'); ws.onmessage = (e)=>{console.log(e.data);}");

    //load textures using TextureUtils
    game.game.Textures.renderer = renderer;
    game.game.Textures.loadTextures();

    game.game.WorldGen.city_noise.SetSeed(rand() % 10000);
    game.game.WorldGen.terrain_noise.SetSeed(rand() % 10000);
    game.game.WorldGen.fish_noise.SetSeed(rand() % 10000);
    game.game.WorldGen.tree_noise.SetSeed(rand() % 10000);

    //Entity creation
    for(unsigned int i = 0; i < 200; ++i){
        float n = game.game.noise.GetPerlinFractal(i, -i);
        Entity e {
            .speed = (static_cast<float>((rand() % 30 < 5) ? (rand() % 15) + 15 : rand() % 15)),
            .size = 10, .directionx = 1 - ((rand() % 10 < 5) ? 1 : 0), .directiony = 1 - ((rand() % 10 < 5) ? 1 : 0), 
            .color = SDL_Color{static_cast<Uint8>(rand() % 256), static_cast<Uint8>(rand() % 256), static_cast<Uint8>(rand() % 256)},
            .persist = false, .timex = 0, .timey = 0, .index = i, .hasTex = true, .texIndex = rand() % MAX_char, .texAng = 0,
            .ID = game.rstring(10), .boat_texIndex = static_cast<unsigned int>(rand() % 6) + 18, .fast_texIndex = (rand() % 6) + (static_cast<int>(game.game.Textures.Textures["characters"].size()) - 6),
            .health = 100.f
        };
        
        if(e.speed < 20){
            e.fast_texIndex = e.texIndex;
        }
        Position entity_spawn = Position{static_cast<float>(rand() % 10000), static_cast<float>(rand() % 10000)};

        //Position entity_spawn = Position{static_cast<float>((rand()%100) - fmod(n*400, 100000)*1000), static_cast<float>((rand()%100)*fmod(n*400, 100000)*1000 - fmod(n*400, 10000)*1000)};
        e.chunk = game.getChunkFromCoord(entity_spawn.x, entity_spawn.y);
        n = (game.game.noise.GetPerlin((e.chunk.x), (e.chunk.y)) - -1) / (1 - -1);
        n = (game.game.noise.GetPerlinFractal((e.chunk.x)+pow(n,2), (e.chunk.y)+pow(n,2)) - -1) / (1 - -1);
        if(n < .45){
            e.items["boat"] = 1;
        }
        if(e.fast_texIndex > game.game.Textures.Textures["characters"].size() - 2){
            e.items["fly"] = 1;
        }
        e.temp_speed = e.speed;
        e.position.x = entity_spawn.x;
        e.position.y = entity_spawn.y;
        game.game.entities.push_back(e);
    }


    emscripten_set_main_loop_arg(mainloop, &ctx, fps, simulate_infinite_loop);
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
