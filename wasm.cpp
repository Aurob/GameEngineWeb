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
#include "wordgen.h"

struct context
{
    SDL_Renderer *renderer;
    int iteration;
};

struct Position
{
    float x;
    float y;
    bool visible;
};

struct Entity
{
    Position position;
    Position chunk;
    float speed;
    float size;
    int directionx;
    int directiony;
    float chunkfx;
    float chunkfy;
    SDL_Color color;
    bool persist;
    unsigned int oldtime;
    unsigned int newtime;
    float timex;
    float timey;
    unsigned int index;
    bool hasTex;
    int texIndex;
    int step;
    int texAng;
    std::string ID;
    std::unordered_map<std::string, int> items;
    unsigned int boat_texIndex;
    int fast_texIndex;
    float temp_speed;
};

struct GameOBJ
{
    float speed;
    float xoffset;
    float yoffset;
    int xchunk; //used for mouse clicks
    int ychunk; //.
    int zoom_modx;
    int zoom_mody;
    unsigned int width;
    unsigned int height;
    std::vector<int> chunk_sizes;
    std::vector<Entity> entities;
    unsigned int current_chunk_size;
    unsigned int chunk_size;
    unsigned int size;
    unsigned int time = SDL_GetTicks();
    unsigned int MAX_ENTITIES;
    FastNoise noise{};
    std::vector<Entity> visible_entities;
    int default_chk;
    unsigned int max_fish = 30;
    float time_stepx;
    float time_stepy;
    Entity mouse_entity;
};

struct User
{
    float globalx;
    float globaly;
    int chunks[4][2];
    std::unordered_map<std::string, int> mouse;
    int chunk[2];
    int mouse_chunk[2];
    std::unordered_map<int, bool> keyState;
    std::unordered_map<std::string, Position> clicks;
    std::unordered_map<std::string, Entity> owned_entities;
    bool mouse_down;
};

struct Texture
{
    std::string name;
    int w;
    int h;
    SDL_Texture *tex = NULL;

};

std::string alphanum{"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"};
std::vector<std::string> image_files{
  "Resources/grass2.png",
  "Resources/battery.png",
  "Resources/binoculars.png",
  "Resources/book.png",
  "Resources/bottle.png",
  "Resources/bunsen.png",
  "Resources/camera.png",
  "Resources/double_welder.png",
  "Resources/drill.png",
  "Resources/extinguisher.png",
  "Resources/flashlight.png",
  "Resources/game.png",
  "Resources/gasmask.png",
  "Resources/hacksaw.png",
  "Resources/hammer.png",
  "Resources/icecream.png",
  "Resources/knife.png",
  "Resources/lantern.png",
  "Resources/medkit.png",
  "Resources/microscope.png",
  "Resources/plant.png",
  "Resources/pliers.png",
  "Resources/pump.png",
  "Resources/radio.png",
  "Resources/sample.png",
  "Resources/screwdriver.png",
  "Resources/soylent.png",
  "Resources/spool.png",
  "Resources/syringe.png",
  "Resources/tablet.png",
  "Resources/thermos.png",
  "Resources/tileGrass1.png",
  "Resources/welder.png",
  "Resources/wrench.png"
};

std::vector<std::string> char_files{
    "Resources/character/Alchemist_walk.png",
    "Resources/character/Barmaid_walk.png",
    "Resources/character/Bartender_walk.png",
    "Resources/character/Blacksmith_walk.png",
    "Resources/character/Farmer_walk.png",
    "Resources/character/Fisherman_walk.png",
    "Resources/character/Kid01_walk.png",
    "Resources/character/Kid02_walk.png",
    "Resources/character/Merchant_walk.png",
    "Resources/character/Alchemist_idle.png",
    "Resources/character/Barmaid_idle.png",
    "Resources/character/Bartender_idle.png",
    "Resources/character/Blacksmith_idle.png",
    "Resources/character/Farmer_idle.png",
    "Resources/character/Fisherman_idle.png",
    "Resources/character/Kid01_idle.png",
    "Resources/character/Kid02_idle.png",
    "Resources/character/Merchant_idle.png",
    "Resources/character/pipo-submarine_2.png",
    "Resources/character/pipo-boat_nosail.png",
    "Resources/character/pipo-boat_1.png",
    "Resources/character/pipo-boat_b_1.png",
    "Resources/character/pipo-ship_c_1.png",
    "Resources/character/pipo-ship_d_1.png",
    "Resources/character/pipo-van_1.png",
    "Resources/character/pipo-van_b_1.png",
    "Resources/character/pipo-van_c_1.png",
    "Resources/character/pipo-balloon_1.png",
    "Resources/character/pipo-balloon_b_1.png",
    "Resources/character/pipo-balloon_c_1.png"
};
const int MAX_char = 8;

std::vector<std::string> tile_files {
    "Resources/tiles/[A]Grass_pipo.png",
    "Resources/tiles/[A]Dirt1-Dirt2_pipo.png",
    "Resources/tiles/[A]Dirt1-Dirt3_pipo.png",
    "Resources/tiles/[A]Grass1-Dirt4_pipo.png",
    "Resources/tiles/[A]Grass1-Dirt1_pipo.png",
    "Resources/tiles/[A]Grass1-Dirt2_pipo.png",
    "Resources/tiles/[A]Grass1-Dirt3_pipo.png",
    "Resources/tiles/[A]Grass1-Dirt4_pipo.png",
    "Resources/tiles/[A]Grass1-Grass2_pipo.png",
    "Resources/tiles/[A]Grass1-Grass3_pipo.png",
    "Resources/tiles/[A]Grass1-Grass4_pipo.png",
    "Resources/tiles/[A]LongGrass_pipo.png",
    "Resources/tiles/[A]Water4_pipo.png",
    "Resources/tiles/[A]Water2_pipo.png"
};


std::vector<std::string> popup_files {
    "Resources/world_popup/pipo-popupemotes.png",
};

//Begin defining game settings
GameOBJ game {
    .speed = 10,
    .xoffset = 0, .yoffset = 0,
    .width = 2000, .height = 1000,
    .chunk_sizes = std::vector<int>{10, 20, 50, 100, 250, 500},
    .current_chunk_size = 1, .chunk_size = 20, .size = 25,
    .time = SDL_GetTicks(), .MAX_ENTITIES = 10, .default_chk = 3,
    .time_stepx = 0, .time_stepy = 0
};

User user {
    .globalx = 1, .globaly = 1,
    .mouse{{"x",0},{"y",0}}, .chunk{0, 0},
    .mouse_chunk{0,0}, 
};

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

EM_JS(int, get_icon_index, (), {
    return getIcon();
});

extern "C" {
    int ecount(){
        return user.owned_entities.size();
    }
    const char* get_info(int type){
        const char* retval;
        FastNoise noise;
        switch(type){
            case 0:
                retval = (std::to_string(user.mouse["x"]) + ", " + std::to_string(user.mouse["y"])).c_str();
                break;

            case 1:
                retval = (std::to_string(static_cast<int>(user.globalx)) + ", " + std::to_string(static_cast<int>(user.globaly))).c_str();
                break;

            case 2:
                retval = (std::to_string(user.mouse_chunk[0]) + ", " + std::to_string(user.mouse_chunk[1])).c_str();
                break;
            case 3:
                retval = std::to_string(noise.GetPerlin(user.chunk[0], user.chunk[1])).c_str();
                break;
            case 4:
                retval = game.mouse_entity.ID.c_str();
               // retval = genword(true, true).c_str();
                break;
            default:
                retval = "No type specified";
                break;
        }
        return retval;
    }
}



//returns the screen x,y coordinate of a specified chunk
Position content(Position chunk, int bg_render = 0){

    int dx = (game.xoffset < 0) ? game.chunk_size + game.xoffset : game.xoffset;
    int dy = (game.yoffset < 0) ? game.chunk_size + game.yoffset : game.yoffset;

    if(user.chunks[0][0] - bg_render <= chunk.x && user.chunks[1][0] + bg_render >= chunk.x && user.chunks[0][1] - bg_render <= chunk.y && user.chunks[2][1] + bg_render >= chunk.y){
        return Position{
            -dx + (chunk.x - user.chunks[0][0])*static_cast<int>(game.chunk_size),
            -dy + (chunk.y - user.chunks[0][1])*static_cast<int>(game.chunk_size),
            true
        };
    }
    else chunk.visible = false;
    return chunk;
}

//Returns the chunk (x, y) of a specified global position (x, y)
Position getChunkFromCoord(float x, float y) {
    float xchunk = 0;
    float ychunk = 0;
    if (game.current_chunk_size <= game.default_chk-2 || game.current_chunk_size == game.default_chk+2 || game.current_chunk_size == game.default_chk) {
        xchunk = floor((x) / static_cast<float>(game.chunk_sizes[game.default_chk]));
        ychunk = floor((y) / static_cast<float>(game.chunk_sizes[game.default_chk]));
    }
    else if(game.current_chunk_size != game.default_chk){
        xchunk = floor(((x) / fmod(static_cast<float>(game.chunk_size), static_cast<float>(game.chunk_sizes[game.default_chk]))) / 2);
        ychunk = floor(((y) / fmod(static_cast<float>(game.chunk_size), static_cast<float>(game.chunk_sizes[game.default_chk]))) / 2);
    }
    
    return Position{ xchunk, ychunk, true };
}


//Batch updates game values
void update_pos(){
    //update global position
    if(user.keyState[1]) user.globalx+=game.speed * ((user.keyState[5]) ? 15 : 1); //D
    if(user.keyState[2]) user.globalx-=game.speed * ((user.keyState[5]) ? 15 : 1); //A
    if(user.keyState[3]) user.globaly+=game.speed * ((user.keyState[5]) ? 15 : 1); //S
    if(user.keyState[4]) user.globaly-=game.speed * ((user.keyState[5]) ? 15 : 1); //W

    //update camera offsets
    if(game.current_chunk_size != game.default_chk){
        game.xoffset = (fmod(user.globalx, static_cast<float>(game.chunk_sizes[game.default_chk])) / static_cast<float>(game.chunk_sizes[game.default_chk])) * static_cast<float>(game.chunk_size);
        game.yoffset = (fmod(user.globaly, static_cast<float>(game.chunk_sizes[game.default_chk])) / static_cast<float>(game.chunk_sizes[game.default_chk])) * static_cast<float>(game.chunk_size);

        game.zoom_modx = floor(user.globalx / static_cast<float>(game.chunk_sizes[game.default_chk])) - floor(user.globalx / static_cast<float>(game.chunk_size));
        game.zoom_mody = floor(user.globaly / static_cast<float>(game.chunk_sizes[game.default_chk])) - floor(user.globaly / static_cast<float>(game.chunk_size));
    }
    else{
        game.xoffset = floor(fmod(user.globalx, static_cast<float>(game.chunk_size)));
        game.yoffset = floor(fmod(user.globaly, static_cast<float>(game.chunk_size)));
        game.zoom_modx = 0;
        game.zoom_mody = 0;
    }

    int xchunk1 = floor((user.globalx - (static_cast<float>(game.width)/2)) / static_cast<float>(game.chunk_size)) + game.zoom_modx;
    int xchunk2 = xchunk1 + (static_cast<float>(game.width) / static_cast<float>(game.chunk_size));
    int ychunk1 = floor((user.globaly - (static_cast<float>(game.height)/2)) / static_cast<float>(game.chunk_size)) + game.zoom_mody;
    int ychunk2 = ychunk1 + (static_cast<float>(game.height) / static_cast<float>(game.chunk_size));

    user.chunks[0][0] = xchunk1; user.chunks[0][1] = ychunk1;

    user.chunks[1][0] = xchunk2; user.chunks[1][1] = ychunk1;

    user.chunks[2][0] = xchunk1; user.chunks[2][1] = ychunk2;

    user.chunks[3][0] = xchunk2; user.chunks[3][1] = ychunk2;

    user.chunk[0] = user.chunks[0][0] + floor(static_cast<float>(user.chunks[1][0] - user.chunks[0][0]) / 2);
    user.chunk[1] = user.chunks[0][1] + floor(static_cast<float>(user.chunks[2][1] - user.chunks[0][1]) / 2);

    for(int i = 0; i < floor(static_cast<float>(game.width)/static_cast<float>(game.chunk_size)) + 1; i++){
        int xpos = i*game.chunk_size - game.xoffset;
        if(user.mouse["x"] > xpos){
            user.mouse_chunk[0] = xpos;
        }
    }
    for(int i = 0; i < floor(static_cast<float>(game.height)/static_cast<float>(game.chunk_size)) + 1; i++){    
        int ypos = i*game.chunk_size - game.yoffset;
        if(user.mouse["y"] > ypos){
            user.mouse_chunk[1] = ypos;
        }
    }

    // if(user.mouse_down){
    //     //uchunk = content(Position{static_cast<float>(user.chunk[0]), static_cast<float>(user.chunk[1])});
    //     int xchunk = ceil((user.mouse_chunk[0] / game.chunk_size) + user.chunks[0][0]) + 1;// + ((user.chunk[0] <= 0) ? 0 : 1);
    //     int ychunk = ceil((user.mouse_chunk[1] / game.chunk_size) + user.chunks[0][1]) + 1;// + ((user.chunk[1] <= 0) ? 0 : 1);
    //     //xchunk += uchunk.x;
    //     //ychunk += uchunk.y;
    //     std::string chunk_key = std::to_string(xchunk)+std::to_string(ychunk);
        

    //     // Searching for chunk_key in clicks
    //     if(user.clicks.find(chunk_key) != user.clicks.end()){
    //         user.clicks.erase(chunk_key);
    //     }
    //     else{
    //         user.clicks[chunk_key] aw= Position{static_cast<float>(xchunk), static_cast<float>(ychunk)};
    //     }
    // }


}

bool zorder(const Entity &a, const Entity &b){
    return a.position.y < b.position.y;
}
//loop through current entities and update each
//despawn any entites outside of render distance
//update positions for visible entities
void update_entities(){
    std::sort(game.entities.begin(), game.entities.end(), zorder);
    unsigned int index{};
    float n;
    game.visible_entities.clear();
    for(Entity& entity : game.entities){

        if(entity.ID == game.mouse_entity.ID && user.mouse_down){ // 
            //Do something with the entity
            entity.speed = 0;
        }else entity.speed = entity.temp_speed;

        //Check if the entity is within 5 chunks of the user's visible range
        if(content(entity.chunk, 10).visible){
            game.visible_entities.push_back(entity);
            Position old_pos = Position{entity.position.x, entity.position.y};

            entity.position.x += (entity.speed * entity.directionx);// + (n * 2);
            entity.position.y += (entity.speed * entity.directiony);// + (n * 2);
            entity.chunk = getChunkFromCoord(entity.position.x, entity.position.y);

            n = (game.noise.GetPerlin((entity.chunk.x), (entity.chunk.y)) - -1) / (1 - -1);
            n = (game.noise.GetPerlinFractal((entity.chunk.x)+pow(n,2), (entity.chunk.y)+pow(n,2)) - -1) / (1 - -1);

            if(n < .45 && entity.items["boat"] < 1){
                entity.position.x = old_pos.x;
                entity.position.y = old_pos.y;
                
                entity.directionx *= (rand() % 100 < 20) ? -1 : 1;
                entity.directiony *= (rand() % 100 < 20) ? -1 : 1;
                entity.timex *= 10;
                entity.timey *= 10;
                entity.chunk = getChunkFromCoord(entity.position.x, entity.position.y);
            }

            entity.chunkfx = abs((entity.chunk.x * game.chunk_sizes[3]) - (entity.position.x)) / game.chunk_sizes[3];
            entity.chunkfy = abs((entity.chunk.y * game.chunk_sizes[3]) - (entity.position.y)) / game.chunk_sizes[3];

            //set walk direction and update texture
            if(rand() % 1000 < 5){
                entity.directionx *= -1;
                entity.timex = 0;
                if(entity.directionx > 0) entity.texAng = 2;
                if(entity.directionx < 0) entity.texAng = 1;
                // if(entity.directionx > 0 && entity.directiony > 0) entity.texAng = (rand() % 10 < 5) ? 2 : 0;
                // if(entity.directionx < 0 && entity.directiony < 0) entity.texAng = (rand() % 10 < 5) ? 1 : 3;
                // if(entity.directionx < 0 && entity.directiony > 0) entity.texAng = (rand() % 10 < 5) ? 1 : 0;
                // if(entity.directionx > 0 && entity.directiony < 0) entity.texAng = (rand() % 10 < 5) ? 2 : 3;
            }
            if(rand() % 1000 < 5){
                entity.directiony *= -1;
                entity.timey = 0;
                // if(entity.directionx > 0 && entity.directiony > 0) entity.texAng = (rand() % 10 < 5) ? 2 : 0;
                // if(entity.directionx < 0 && entity.directiony < 0) entity.texAng = (rand() % 10 < 5) ? 1 : 3;
                // if(entity.directionx < 0 && entity.directiony > 0) entity.texAng = (rand() % 10 < 5) ? 1 : 0;
                // if(entity.directionx > 0 && entity.directiony < 0) entity.texAng = (rand() % 10 < 5) ? 2 : 3;
                if(entity.directiony > 0) entity.texAng = 0;
                if(entity.directiony < 0) entity.texAng = 3;
            }

            //change the speed of step change based on entity speed
            //fast moving entities should change animations quicker
            //entity.speed
            //n = game.noise.GetPerlinFractal(entity.position.x, entity.position.y);
            //entity.speed = n*25;

            entity.step = (rand() % 1000 < n *50) ? (entity.step+1) : entity.step;//(rand() % 500 < 100 * entity.speed) ? floor((100 * entity.speed) / 25) : entity.step;
            entity.step%=4;
            entity.timex += .5;
            entity.timey += .5;

            
        }
        ++index;
    }
}

std::string rstring(size_t length){
    std::string new_key{""};
    for(int i = length; i >= 0; --i){
        new_key += static_cast<std::string>(alphanum).at(rand() % 42);
    }
    return new_key;
}

int SDLCALL EventHandler(void *userdata, SDL_Event *event) {
    int xchunk, ychunk;
    std::string chunk_key;
    Position p;
    temp_click = {};
    switch(event->type) {
        case SDL_MOUSEMOTION:
            user.mouse["x"] = event->motion.x;
            user.mouse["y"] = event->motion.y;

            break;

        case SDL_MOUSEWHEEL:
            if(event->wheel.y < 0 && game.current_chunk_size > 0) game.current_chunk_size--;
            if(event->wheel.y > 0 && game.current_chunk_size < 5) game.current_chunk_size++;
            
            game.chunk_size = game.chunk_sizes[game.current_chunk_size];

            game.size = floor(static_cast<float>(game.chunk_size) / 2);

            break;

        case SDL_MOUSEBUTTONDOWN:
            user.mouse_down = true;
            
            break;   

        case SDL_MOUSEBUTTONUP:
            user.mouse_down = false;
            break;

        case SDL_KEYUP:

            switch (event->key.keysym.sym) {
                case SDLK_d: user.keyState[1] = false; break;
                case SDLK_a: user.keyState[2] = false; break;
                case SDLK_s: user.keyState[3] = false; break;
                case SDLK_w: user.keyState[4] = false; break;
                case SDLK_LSHIFT: user.keyState[5] = false; break;
                default: break;
            }
            break;

        case SDL_KEYDOWN:
            switch (event->key.keysym.sym) {
                case SDLK_d: user.keyState[1] = true; break;
                case SDLK_a: user.keyState[2] = true; break;
                case SDLK_s: user.keyState[3] = true; break;
                case SDLK_w: user.keyState[4] = true; break;
                case SDLK_LSHIFT: user.keyState[5] = true; break;
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
    game.time_stepx += (rand() % 100 < 5) ? -.01 : .01;
    game.time_stepy += (rand() % 100 < 5) ? -.01 : .01;
    update_pos();
    update_entities();

    //Draw grid with offsets    
    // Draw grid background.
    SDL_SetRenderDrawColor(renderer, 173, 193, 217, 255);
    SDL_RenderClear(renderer);

    // Draw grid lines.
    SDL_SetRenderDrawColor(renderer, 255, 255,
                            255, 255);

    SDL_Rect tiletexr;
    SDL_Rect steptexr; 
    // for (float x = 0; x <= static_cast<float>(game.width)/static_cast<float>(game.chunk_size); ++x) {
    //     xpos = ((x*static_cast<float>(game.chunk_size)) - ((user.chunk[0] < 0) ? 1 : 0)) - game.xoffset;
    //     for (float y =0; y <= static_cast<float>(game.height)/static_cast<float>(game.chunk_size); ++y) {
    //         ypos = ((y*static_cast<float>(game.chunk_size)) - ((user.chunk[1] < 0) ? 1 : 0)) - game.yoffset;




    //Picks a tile type from each viewable chunk/tile
    //selects a tile type based on noise value
    //spawns spawnable entities
    for (int i = user.chunks[0][0]; i < user.chunks[1][0] + 1; i++) {
        for (int j = user.chunks[0][1]; j < user.chunks[3][1] + 1; j++) {
            
            Position chunk_position = content(Position{static_cast<float>(i), static_cast<float>(j)});

            float n = (game.noise.GetPerlin((i), (j)) - -1) / (1 - -1);
            n = (game.noise.GetPerlinFractal((i)+pow(n,2), (j)+pow(n,2)) - -1) / (1 - -1);

            //draw user sprite;

            if (n < 0.45) biometex = 13; //water
            else if (n < 0.46) biometex = 2;
            else if (n < 0.48) biometex = 1;
            else if (n < 0.50) biometex = 5;
            else if (n < 0.52) biometex = 4;
            else if (n < 0.67) biometex = 8;
            else biometex = 3;

            if(biometex == 12){
                tiletexr.x = chunk_position.x; tiletexr.y = chunk_position.y;
                tiletexr.w = game.chunk_size; tiletexr.h = game.chunk_size; 
                steptexr.x = 32; steptexr.y = 0;
                steptexr.w = 32; steptexr.h = 32; 
            }
            else if(biometex == 13 || biometex == 8 || biometex == 3){
                tiletexr.x = chunk_position.x; tiletexr.y = chunk_position.y;
                tiletexr.w = game.chunk_size; tiletexr.h = game.chunk_size; 
                steptexr.x = 192; steptexr.y = 32;
                steptexr.w = 32; steptexr.h = 32; 
            }
            else{
                tiletexr.x = chunk_position.x; tiletexr.y = chunk_position.y;
                tiletexr.w = game.chunk_size; tiletexr.h = game.chunk_size; 
                steptexr.x = 0; steptexr.y = 32;
                steptexr.w = 64; steptexr.h = 64; 
            }

            //Draw tile
            SDL_RenderCopy(renderer, tileicons[biometex].tex, &steptexr, &tiletexr);

            //Render city spawns with Cellular noise
            //minimal multipliers
            if(biometex == 8){
                n = (game.noise.GetCellular((i)*2, (j)*2));
                if(n > .85){
                    FastNoise tempnoise;
                    tempnoise.SetFrequency(.04);
                    tempnoise.SetFractalOctaves(32);
                    tempnoise.SetFractalLacunarity(0.0);
                    tempnoise.SetFractalType(FastNoise::RigidMulti);

                    n = (tempnoise.GetSimplex((i)*2, (j)*2));
                    srand(i*j);
                    if(n > .10 && n < .30) SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255 );
                    
                    else SDL_SetRenderDrawColor(renderer, 124, 224, 42, 125 );
                    SDL_RenderFillRect(renderer, &tiletexr);
                }
            }
            //Draw fish popups on water tiles
            n = (game.noise.GetCellular((game.time_stepx+i)*100, (game.time_stepy+j)*100) - -1) / (1 - -1);
            if(biometex == 13 && (n <.5 && n > .4) && fmod(n, static_cast<int>((n*100))) < 4){
                tiletexr.x = chunk_position.x + (game.chunk_size/1.5)/3; tiletexr.y = (chunk_position.y) + (game.chunk_size/1.5)/4;
                tiletexr.w = game.chunk_size/1.5; tiletexr.h = game.chunk_size/1.5; 
                steptexr.x = 96; steptexr.y = 192;
                steptexr.w = 32; steptexr.h = 32; 
                SDL_RenderCopy(renderer, popupicons[0].tex, &steptexr, &tiletexr);
            }
        }
    }

    // // fill the chunk the user is currently in
    SDL_Rect u;
    Position uchunk = content(Position{static_cast<float>(user.chunk[0]), static_cast<float>(user.chunk[1])});
    u.x = uchunk.x + 1;
    u.y = uchunk.y + 1;
    u.w = game.chunk_size - 1;
    u.h = game.chunk_size - 1;
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 128 );
    SDL_RenderFillRect(renderer, &u );
    
    //mouse chunk
    SDL_Rect r;
    r.x = user.mouse_chunk[0];
    r.y = user.mouse_chunk[1];
    r.w = game.chunk_size+1;
    r.h = game.chunk_size+1;
    SDL_SetRenderDrawColor(renderer, 221, 44, 112, 50 );
    SDL_RenderFillRect(renderer, &r);
    SDL_Rect texr; 
    SDL_Rect chartexr;

    // user position is always in the middle of the screen
    SDL_Rect p;
    p.x = static_cast<float>(game.width)/2;
    p.y = static_cast<float>(game.height)/2;
    p.w = game.size;
    p.h = game.size;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255 );
    SDL_RenderFillRect(renderer, &p );

    //draw user content
    for(const auto & [ key, value ] : user.clicks){
        // click rectangle
        SDL_Rect c;
        Position chunk = content(value);
        if(chunk.visible){
            c.x = chunk.x+1;
            c.y = chunk.y+1;
            c.w = game.chunk_size-1;
            c.h = game.chunk_size-1;

            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 50 );
            
            SDL_RenderFillRect(renderer, &c );

            float xpos;float ypos;
            SDL_SetRenderDrawColor(renderer, 0, 20, 0, 50 );
            SDL_RenderDrawLine(renderer, chunk.x+1, chunk.y+1, chunk.x+(game.chunk_size/3), chunk.y+game.chunk_size);
            //SDL_RenderDrawLine(renderer, chunk.x+2, chunk.y+2, game.chunk_size*100, game.chunk_size*100);

            
        }
    }
    //float xpos;
    // for (float x = value.x; x <= static_cast<float>(game.chunk_size)/static_cast<float>(game.chunk_size); ++x) {
    //     xpos = x*static_cast<float>(game.chunk_size) - game.xoffset;
    //     SDL_RenderDrawLine(renderer, xpos, 0, xpos, game.height);
    // }

    // float ypos;
    // for (float y = 0; y <= static_cast<float>(game.height)/static_cast<float>(game.chunk_size); ++y) {
    //     ypos = y*static_cast<float>(game.chunk_size) - game.yoffset;
    //     SDL_RenderDrawLine(renderer, 0, ypos, game.width, ypos);
    // }  

    
    
    
    //draw visible entities
    float n;
    int ei;
    for(Entity& entity : game.visible_entities){
        //SDL_Rect e;
        //if(entity.chunk.x == temp_click.x && entity.chunk.y == temp_click.y){
            user.owned_entities[entity.ID] = entity;
            //user.owned_entities[entity.ID].index = user.owned_entities.size()-1;
            //game.entities.erase(game.entities.begin() + entity.index);
            //update each entities index after the deleted 
            // for(int i = entity.index; i < game.entities.size(); ++i){
            //     game.entities[i].index = i - 1;
            // }
       // }
        Position p = content(entity.chunk);
        if(p.visible){
            

            //draw entity sprite
            float texheight = (game.chunk_size) + ((game.chunk_size) * .333);
            chartexr.x = (p.x + (entity.chunkfx * game.chunk_size)) - (game.size * .7); chartexr.y = (p.y + (entity.chunkfy * game.chunk_size)) - texheight;
            chartexr.w = game.chunk_size; chartexr.h = texheight;

            steptexr.x = entity.step * 32; steptexr.y = entity.texAng * 48;
            steptexr.w = 32; steptexr.h = 48; 
            ei = entity.texIndex + 8;

            // copy the texture to the rendering context
            n = (game.noise.GetPerlin((entity.chunk.x), (entity.chunk.y)) - -1) / (1 - -1);
            n = (game.noise.GetPerlinFractal((entity.chunk.x)+pow(n,2), (entity.chunk.y)+pow(n,2)) - -1) / (1 - -1);
            if(n < .45 && entity.items["boat"] > 0){
                if(entity.speed > 20 && entity.items["fly"] > 0) ei = entity.fast_texIndex;
                else ei = entity.boat_texIndex;
                chartexr.x = (p.x + (entity.chunkfx * game.chunk_size)) - game.chunk_size;
                chartexr.y = (p.y + (entity.chunkfy * game.chunk_size)) - game.chunk_size*3;

                chartexr.w = game.chunk_size*2; chartexr.h = texheight*2;
                steptexr.x = 32 * (entity.step % 3); steptexr.y = entity.texAng * 32;
                steptexr.w = 32; steptexr.h = 32; 
                
            }
            else if(entity.speed > 20){
                ei = entity.fast_texIndex;
                chartexr.x = (p.x + (entity.chunkfx * game.chunk_size)) - game.chunk_size;
                chartexr.y = (p.y + (entity.chunkfy * game.chunk_size)) - game.chunk_size*3;
                chartexr.w = game.chunk_size*2; chartexr.h = texheight*2;
                steptexr.x = 32; steptexr.y = entity.texAng * 32;
                steptexr.w = 32; steptexr.h = 32; 
            }

            if(game.mouse_entity.ID == entity.ID){
                e.x = p.x + (entity.chunkfx * game.chunk_size);
                e.y = p.y + (entity.chunkfy * game.chunk_size);
                e.w = game.chunk_size;
                e.h = game.chunk_size;
                SDL_SetRenderDrawColor(renderer, entity.color.r, entity.color.g, entity.color.b, 255 );
                SDL_RenderFillRect(renderer, &e);
            }
            
            SDL_RenderCopy(renderer, charicons[ei].tex, &steptexr, &chartexr);

            if(chartexr.x < user.mouse["x"] && user.mouse["x"] < chartexr.x + chartexr.w){
                if(chartexr.y < user.mouse["y"] && user.mouse["y"] < chartexr.y+ chartexr.h){
                    game.mouse_entity = entity;
                    if(user.mouse_down){
                        chartexr.x = (p.x + (entity.chunkfx * game.chunk_size)) - (game.size * .7); chartexr.y = (p.y + (entity.chunkfy * game.chunk_size)) - texheight;
                        chartexr.w = game.chunk_size/3; chartexr.h = game.chunk_size/3;
                        steptexr.x = 0; steptexr.y = 0;
                        steptexr.w = 32; steptexr.h = 32; 
                        SDL_RenderCopy(renderer, popupicons[0].tex, &steptexr, &chartexr);
                    }
                    
                }
            }
        }
    }
 

    

    // mouse position rectangle
    SDL_Rect m;
    m.x = user.mouse["x"];
    m.y = user.mouse["y"];
    m.w = 5;
    m.h = 5;
    SDL_SetRenderDrawColor(renderer, 0, 0, 123, 255 );
    SDL_RenderFillRect(renderer, &m );
    //draw user sprite
    texr.x = user.mouse["x"] - (game.chunk_size*.8)/2;//static_cast<float>(game.width)/2; 
    texr.y = user.mouse["y"] - (game.chunk_size*.8)/2;//static_cast<float>(game.height)/2;
    texr.w = game.chunk_size*.8; texr.h = game.chunk_size*.8; 
    // copy the texture to the rendering context
    SDL_RenderCopy(renderer, icons[get_icon_index()].tex, NULL, &texr);

    SDL_RenderPresent(renderer);
    ctx->iteration++;
}

int main(int argc, char *argv[])
{
    //seed generator
    srand(time(NULL));
    game.noise.SetSeed(rand() % 10000);
    
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_CreateWindowAndRenderer(game.width, game.height, 0, &window, &renderer);

    context ctx;
    ctx.renderer = renderer;
    ctx.iteration = 0;

    const int simulate_infinite_loop = 1; // call the function repeatedly
    const int fps = -1;//-1; // call the function as fast as the browser wants to render (typically 60fps)
    //emscripten_run_script("var ws = new WebSocket('wss://robauis.me/ws'); ws.onmessage = (e)=>{console.log(e.data);}");

    for(unsigned int i = 0; i < 280; ++i){
        float n = game.noise.GetPerlinFractal(i, -i);
        Entity e {
            .speed = (static_cast<float>((rand() % 30 < 5) ? (rand() % 15) + 15 : rand() % 15)),
            .size = 10, .directionx = 1 - ((rand() % 10 < 5) ? 1 : 0), .directiony = 1 - ((rand() % 10 < 5) ? 1 : 0), 
            .color = SDL_Color{static_cast<Uint8>(rand() % 256), static_cast<Uint8>(rand() % 256), static_cast<Uint8>(rand() % 256)},
            .persist = false, .timex = 0, .timey = 0, .index = i, .hasTex = true, .texIndex = rand() % MAX_char, .texAng = 0,
            .ID = rstring(10), .boat_texIndex = static_cast<unsigned int>(rand() % 6) + 18, .fast_texIndex = (rand() % 6) + (static_cast<int>(char_files.size()) - 6)
        };
        
        if(e.speed < 20){
            e.fast_texIndex = e.texIndex;
        }
        Position entity_spawn = Position{static_cast<float>(rand() % 10000 - rand() % 10000)*(n*10), static_cast<float>(rand() % 10000 - rand() % 10000)*(n*10)};
        //Position entity_spawn = Position{static_cast<float>((rand()%100) - fmod(n*400, 100000)*1000), static_cast<float>((rand()%100)*fmod(n*400, 100000)*1000 - fmod(n*400, 10000)*1000)};
        e.chunk = getChunkFromCoord(entity_spawn.x, entity_spawn.y);
        n = (game.noise.GetPerlin((e.chunk.x), (e.chunk.y)) - -1) / (1 - -1);
        n = (game.noise.GetPerlinFractal((e.chunk.x)+pow(n,2), (e.chunk.y)+pow(n,2)) - -1) / (1 - -1);
        if(n < .45){
            e.items["boat"] = 1;
        }
        if(e.fast_texIndex > char_files.size() - 2){
            e.items["fly"] = 1;
        }
        e.temp_speed = e.speed;
        e.position.x = entity_spawn.x;
        e.position.y = entity_spawn.y;
        game.entities.push_back(e);
    }
    //character images
    for(int i = 0; i < char_files.size(); i++){
        img = IMG_LoadTexture(renderer, char_files[i].c_str());
        charicons.push_back(Texture{char_files[i].c_str(), 0, 0, img}); 

        SDL_QueryTexture(charicons[i].tex, NULL, NULL, &charicons[i].w, &charicons[i].h); // get the width and height of the texture
    }
    //item images
    for(int i = 0; i < image_files.size(); i++){
        img = IMG_LoadTexture(renderer, image_files[i].c_str());
        icons.push_back(Texture{image_files[i].c_str(), 0, 0, img}); 

        SDL_QueryTexture(icons[i].tex, NULL, NULL, &icons[i].w, &icons[i].h); // get the width and height of the texture
        
    }
    //tile images
    for(int i = 0; i < tile_files.size(); i++){
        img = IMG_LoadTexture(renderer, tile_files[i].c_str());
        tileicons.push_back(Texture{tile_files[i].c_str(), 0, 0, img}); 

        SDL_QueryTexture(tileicons[i].tex, NULL, NULL, &tileicons[i].w, &tileicons[i].h); // get the width and height of the texture 
    }
    //popup images
    for(int i = 0; i < popup_files.size(); i++){
        img = IMG_LoadTexture(renderer, popup_files[i].c_str());
        popupicons.push_back(Texture{popup_files[i].c_str(), 0, 0, img}); 

        SDL_QueryTexture(popupicons[i].tex, NULL, NULL, &popupicons[i].w, &popupicons[i].h); // get the width and height of the texture 
    }

    emscripten_set_main_loop_arg(mainloop, &ctx, fps, simulate_infinite_loop);
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
