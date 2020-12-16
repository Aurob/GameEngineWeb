#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
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

void mainloop(void *arg)
{   
    SDL_Event event;
    //Handle events
    while (SDL_PollEvent(&event)) {
        EventHandler(0, &event);
    }

    context *ctx = static_cast<context*>(arg);
    SDL_Renderer *renderer = ctx->renderer;

    //Start to update game content
    game.game.time_stepx += (rand() % 100 < 5) ? -.01 : .01;
    game.game.time_stepy += (rand() % 100 < 5) ? -.01 : .01;
    game.update_pos();
    game.update_entities();
    //game.game.EntityManager.update();

    SDL_RenderClear(renderer);

    SDL_Rect tiletexr;
    SDL_Rect steptexr; 
    
    SDL_Rect texr; 
    SDL_Rect chartexr;

    //Picks a tile type from each viewable chunk/tile
    //selects a tile type based on noise value
    //spawns spawnable entities
    for (int i = game.user.chunks[0][0]; i < game.user.chunks[1][0] + 1; i++) {
        for (int j = game.user.chunks[0][1]; j < game.user.chunks[3][1] + 1; j++) {
            
            //Get the screen coordinates of the current tile
            Position chunk_position = game.content(Position{static_cast<float>(i), static_cast<float>(j)}, 0);
            biometex = game.game.WorldGen.terrainGeneration(i, j);
            
            tiletexr.x = chunk_position.x; tiletexr.y = chunk_position.y;
            tiletexr.w = game.game.chunk_size; tiletexr.h = game.game.chunk_size; 


            //Start by drawing the tile type
            //Will be a terrain tile or city tile
            if(biometex == 12){
                steptexr.x = 32; steptexr.y = 0;
                steptexr.w = 32; steptexr.h = 32; 
            }
            else if(biometex == 13 || biometex == 8 || biometex == 3){
                steptexr.x = 192; steptexr.y = 32;
                steptexr.w = 32; steptexr.h = 32;
            }
            else{
                steptexr.x = 0; steptexr.y = 32;
                steptexr.w = 64; steptexr.h = 64; 
            }
            
            //Draw main tile
            SDL_RenderCopy(renderer, game.game.Textures.Textures["tiles"][biometex].tex, &steptexr, &tiletexr);

            //Next determine secondary tile spawns
            //(fish, trees, rocks, etc..)

            //Generate fish popups on water only
            if(biometex == 13){
                if(game.game.WorldGen.fishGeneration(i, j, game.game.time_stepx, game.game.time_stepy)){
                    tiletexr.x = chunk_position.x + (game.game.chunk_size/1.5)/3; tiletexr.y = (chunk_position.y) + (game.game.chunk_size/1.5)/4;
                    tiletexr.w = game.game.chunk_size/1.5; tiletexr.h = game.game.chunk_size/1.5; 
                    steptexr.x = 96 + ((static_cast<int>(game.game.time_stepx/100)) %3)*32; steptexr.y = 192;
                    steptexr.w = 32; steptexr.h = 32; 
                    SDL_RenderCopy(renderer, game.game.Textures.Textures["popups"][0].tex, &steptexr, &tiletexr);
                } 
            }
            
            bool ignore = false;
            for(int pi = 0; pi < game.game.WorldGen.ignored_tiles.size(); ++pi){
                if(game.game.WorldGen.ignored_tiles[pi][0] == i && game.game.WorldGen.ignored_tiles[pi][1] == j){
                    ignore = true;
                    break;
                }
            }

            if(!ignore){
                if(biometex == 8){
                    if(game.game.WorldGen.treeGeneration(i, j)){
                        steptexr.x = 0; steptexr.y = 32;
                        steptexr.w = 64; steptexr.h = 64;
                        SDL_RenderCopy(renderer, game.game.Textures.Textures["tiles"][14].tex, &steptexr, &tiletexr);
                        
                    } 
                }

                //Rocks spawn on top of stone tiles
                if(biometex == 3){
                    if(game.game.WorldGen.rockGeneration(i, j)){
                        steptexr.x = 32; steptexr.y = 256;
                        steptexr.w = 32; steptexr.h = 32;
                        SDL_RenderCopy(renderer, game.game.Textures.Textures["tiles"][14].tex, &steptexr, &tiletexr);
                        
                    } 
                }
            }
        }
    }
    
    float n;
    int ei;
    for(Entity& entity : game.game.visible_entities){ 
        Position p = game.content(entity.chunk, 0);
        if(p.visible){
            

            //draw entity sprite
            float texheight = (game.game.chunk_size) + ((game.game.chunk_size) * .333);
            chartexr.x = (p.x + (entity.chunkfx * game.game.chunk_size)) - (game.game.size * .7); chartexr.y = (p.y + (entity.chunkfy * game.game.chunk_size)) - texheight;
            chartexr.w = game.game.chunk_size; chartexr.h = texheight;

            steptexr.x = entity.step * 32; steptexr.y = entity.texAng * 48;
            steptexr.w = 32; steptexr.h = 48; 
            ei = entity.texIndex + 8;

            SDL_RenderCopy(renderer, game.game.Textures.Textures["characters"][ei].tex, &steptexr, &chartexr);
        }
    }

    // // // fill the chunk the user is currently in
    SDL_Rect u;
    Position uchunk = game.content(Position{static_cast<float>(game.user.chunk[0]), static_cast<float>(game.user.chunk[1])}, 0);
    u.x = uchunk.x + 1;
    u.y = uchunk.y + 1;
    u.w = game.game.chunk_size - 1;
    u.h = game.game.chunk_size - 1;
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 128 );
    SDL_RenderFillRect(renderer, &u );

    // //mouse chunk
    SDL_Rect r;
    r.x = game.user.mouse_chunk[0];
    r.y = game.user.mouse_chunk[1];
    r.w = game.game.chunk_size+1;
    r.h = game.game.chunk_size+1;
    SDL_SetRenderDrawColor(renderer, 221, 44, 112, 50 );
    SDL_RenderFillRect(renderer, &r);

    
    // mouse position rectangle
    SDL_Rect m;
    m.x = game.user.mouse["x"];
    m.y = game.user.mouse["y"];
    m.w = 5;
    m.h = 5;
    SDL_SetRenderDrawColor(renderer, 0, 0, 123, 255 );
    SDL_RenderFillRect(renderer, &m );

    //draw user sprite
    //SDL_Rect texr;
    texr.x = game.user.mouse["x"] - (game.game.chunk_size*.8)/2;//static_cast<float>(game.game.width)/2; 
    texr.y = game.user.mouse["y"] - (game.game.chunk_size*.8)/2;//static_cast<float>(game.game.height)/2;
    texr.w = game.game.chunk_size/2; texr.h = game.game.chunk_size/2; 
    // copy the texture to the rendering context
    //SDL_RenderCopy(renderer, icons[get_icon_index()].tex, NULL, &texr);
    SDL_RenderCopy(renderer, game.game.Textures.Textures["items"][0].tex, NULL, &texr);


    
    //finally draw everything to the screen
    SDL_RenderPresent(renderer);
    ctx->iteration++;
}

// Begin JS/C bridges

EM_JS(int, get_icon_index, (), {
    return getIcon();
});

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
    for(unsigned int i = 0; i < 280; ++i){
        float n = game.game.noise.GetPerlinFractal(i, -i);
        Entity e {
            .speed = (static_cast<float>((rand() % 30 < 5) ? (rand() % 15) + 15 : rand() % 15)),
            .size = 10, .directionx = 1 - ((rand() % 10 < 5) ? 1 : 0), .directiony = 1 - ((rand() % 10 < 5) ? 1 : 0), 
            .color = SDL_Color{static_cast<Uint8>(rand() % 256), static_cast<Uint8>(rand() % 256), static_cast<Uint8>(rand() % 256)},
            .persist = false, .timex = 0, .timey = 0, .index = i, .hasTex = true, .texIndex = rand() % MAX_char, .texAng = 0,
            .ID = game.rstring(10), .boat_texIndex = static_cast<unsigned int>(rand() % 6) + 18, .fast_texIndex = (rand() % 6) + (static_cast<int>(game.game.Textures.Textures["characters"].size()) - 6)
        };
        
        if(e.speed < 20){
            e.fast_texIndex = e.texIndex;
        }
        Position entity_spawn = Position{static_cast<float>(rand() % 10000 - rand() % 10000)*(n*10), static_cast<float>(rand() % 10000 - rand() % 10000)*(n*10)};
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
