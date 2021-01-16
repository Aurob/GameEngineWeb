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
            //Temporarily disabling zooming while inside
            //if(!game.data.inside){
                if(event->wheel.y < 0 && game.data.current_chunk_size > 0) game.data.current_chunk_size--;
                if(event->wheel.y > 0 && game.data.current_chunk_size < 5) game.data.current_chunk_size++;
                
                game.data.chunk_size = game.data.chunk_sizes[game.data.current_chunk_size];

                game.data.size = floor(static_cast<float>(game.data.chunk_size) / 2);
            //}

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

EM_JS(int, get_seed_value, (), {
    return getSeed();
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
            alert("You slapped the tree!");
            break;
        case 2:
            alert("Can I help you?");
            break;
        case 3:
            alert("It's a book!");
            alert("Too bad you can't read...");
            break;
        
        case 4:
            alert("The door opened...");
            break;

        case 5:
            //Shows a hidden iframe, then closes it after 15 seconds
            document.getElementById("overlay").innerHTML = '<iframe style="margin-left:0px" width="500px" height="100%" src="https://www.youtube.com/embed/dQw4w9WgXcQ?autoplay=1" frameborder="0" allow="autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen=""></iframe>';
            setTimeout(function(){document.getElementById("overlay").innerHTML = ""},30e3);
            break;
        
        case 6:
            alert("You feel a strong urge to enter...");
            document.getElementById("overlay").innerHTML = '<audio autoplay><source src="Resources/ruski.mp3" type="audio/mpeg"></audio>';
            break;
        
        case 7:
            document.getElementById("overlay").innerHTML = "";
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
int roomx, roomy;
void mainloop(void *arg)
{   
    if(!game.data.seeded || game.data.seed != get_seed_value()) {
        //seed generator
        game.data.seed = get_seed_value();
        std::cout << game.data.seed << std::endl;
        srand(game.data.seed);
        game.data.noise.SetSeed(rand() % 10000);
        //load textures using TextureUtils

        game.data.WorldGen.city_noise.SetSeed(rand() % 10000);
        game.data.WorldGen.terrain_noise.SetSeed(rand() % 10000);
        game.data.WorldGen.fish_noise.SetSeed(rand() % 10000);
        game.data.WorldGen.tree_noise.SetSeed(rand() % 10000);
        game.data.WorldGen.rock_noise.SetSeed(rand() % 10000);
        game.data.seeded = true;
    }

    SDL_Event event;
    //Handle events
    while (SDL_PollEvent(&event)) {
        EventHandler(0, &event);
    }

    context *ctx = static_cast<context*>(arg);
    SDL_Renderer *renderer = ctx->renderer;

    //Start to update game content
    game.data.time_stepx += .01;
    game.data.time_stepy += .01;

    if(game.data.inside) game.update_inside();
    else game.update_pos();
    
    //game.data.EntityManager.update();
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255 );
    SDL_RenderClear(renderer);

    SDL_Rect tiletexr;
    SDL_Rect steptexr; 
    
    SDL_Rect texr; 
    SDL_Rect chartexr;
    SDL_Rect temp_rect;
    
    //Picks a tile type from each viewable chunk/tile
    //selects a tile type based on noise value
    //spawns spawnable entities
    
    //std::vector<Position> trees;
    std::vector<std::vector<int>> ignored_tiles;

    std::vector<Position> structures;
    std::vector<Position> trees;
    std::vector<Position> rocks;
    std::vector<Position> fishs;
    std::unordered_map<int, std::vector<Position>> tiles;

    Position uchunk{static_cast<float>(game.user.chunk[0]), static_cast<float>(game.user.chunk[1])};
    uchunk = game.content(uchunk, 1);

    if(game.data.inside) {

        if((game.data.active_interior.user.x >= game.data.chunk_size*3 && game.data.active_interior.user.x < game.data.chunk_size*4) && game.data.active_interior.user.y > game.data.chunk_size*6){
            send_alert(7);
            game.data.inside = false;
        }
        else {

            roomx = (game.data.width - (game.data.chunk_size*6))/2;
            roomy = (game.data.height - (game.data.chunk_size*6))/2;

            
            for (int i = game.user.chunks[0][0]; i < game.user.chunks[1][0] + 1; i++) {
                for (int j = game.user.chunks[0][1]; j < game.user.chunks[3][1] + 1; j++) {
                    tiletexr.x = roomx + (i * game.data.chunk_size); 
                    tiletexr.y = roomy + (j * game.data.chunk_size);
                    tiletexr.w = game.data.chunk_size; tiletexr.h = game.data.chunk_size; 
                    steptexr.x = 0; steptexr.y = 1152;
                    steptexr.w = 32; steptexr.h = 32; 
                    SDL_RenderCopy(renderer, game.data.Textures.Textures["tiles"][4].tex, &steptexr, &tiletexr);
                }
            }
            
            if(game.data.active_interior.type == "book"){
                //book room
                if(game.data.active_interior.items.size() > 0){
                    tiletexr.x = roomx + (game.data.chunk_size * 2.5); 
                    tiletexr.y = roomy + (game.data.chunk_size * 2.5);
                    tiletexr.w = game.data.chunk_size; tiletexr.h = game.data.chunk_size*.625; 
                    steptexr.x = 64; steptexr.y = 3808;
                    steptexr.w = 32; steptexr.h = 20; 
                    SDL_RenderCopy(renderer, game.data.Textures.Textures["tiles"][4].tex, &steptexr, &tiletexr);
                }
            }
            else if(game.data.active_interior.type == "shrek"){
                tiletexr.x = roomx + (3 * game.data.chunk_size);
                tiletexr.y = roomy + (2 * game.data.chunk_size);
                tiletexr.w = game.data.chunk_size*2; tiletexr.h = game.data.chunk_size*2; 
                steptexr.x = 0; steptexr.y = 0;
                steptexr.w = 304; steptexr.h = 282; 

                
                game.data.active_interior.current = SDL_GetTicks();
                if(game.data.active_interior.current < game.data.active_interior.start + 105000){
                    if(game.data.active_interior.current > game.data.active_interior.start + 4000){
                        game.data.active_interior.step += .5;  
                    }
                    if(game.data.active_interior.current > game.data.active_interior.start + 17580){
                        game.data.active_interior.step += 2;  
                    }
                    if(game.data.active_interior.current > game.data.active_interior.start + 1000) game.data.active_interior.step += .1;    
                    SDL_RenderCopy(renderer, game.data.Textures.Textures["shrek"][static_cast<int>(floor(game.data.active_interior.step)) % 139].tex, &steptexr, &tiletexr);
                }
            }

            //Draws user data
            temp_rect.x = roomx + game.data.active_interior.user.x; 
            temp_rect.y = roomy + game.data.active_interior.user.y;
            temp_rect.w = game.data.chunk_size/5;
            temp_rect.h = game.data.chunk_size/5;
            SDL_SetRenderDrawColor(renderer, 231, 134, 34, 255 );
            SDL_RenderFillRect(renderer, &temp_rect );

            tiletexr.x = roomx + (game.data.chunk_size*3); 
            tiletexr.y = roomy + (game.data.chunk_size*6);
            tiletexr.w = game.data.chunk_size; tiletexr.h = game.data.chunk_size; 
            steptexr.x = 160; steptexr.y = 1152;
            steptexr.w = 32; steptexr.h = 32; 
            SDL_RenderCopy(renderer, game.data.Textures.Textures["tiles"][4].tex, &steptexr, &tiletexr);

        }

    }
    else {
        /*Tile renders*/
        SDL_Rect temp_tex, temp_screen;
        for(auto tile : game.data.tiles){
            temp_tex = tile.texture;
            temp_screen = tile.screen;
            SDL_RenderCopy(renderer, game.data.Textures.Textures["tiles"][tile.resource_index].tex, &temp_tex, &temp_screen);
        }

        for(auto fish : game.data.fishs){
            tiletexr.x = fish.x + (game.data.chunk_size/1.5)/3; tiletexr.y = (fish.y) + (game.data.chunk_size/1.5)/4;
            tiletexr.w = game.data.chunk_size/1.5; tiletexr.h = game.data.chunk_size/1.5; 
            steptexr.x = 96 + ((static_cast<int>(game.data.time_stepx/100)) %3)*32; steptexr.y = 192;
            steptexr.w = 32; steptexr.h = 32; 
            SDL_RenderCopy(renderer, game.data.Textures.Textures["popups"][0].tex, &steptexr, &tiletexr);
        }
        
        /*User renders*/
        // fill the chunk the user is currently in
        temp_rect.x = uchunk.x + 1;
        temp_rect.y = uchunk.y + 1;
        temp_rect.w = game.data.chunk_size - 1;
        temp_rect.h = game.data.chunk_size - 1;
        SDL_SetRenderDrawColor(renderer, 34, 234, 134, 128 );
        SDL_RenderFillRect(renderer, &temp_rect );

        // small rectangle for the user position
        temp_rect.x = game.data.width/2 - ((game.data.chunk_size/5)/2);// - (game.data.size * .7); 
        temp_rect.y = game.data.height/2;// - texheight;
        temp_rect.w = game.data.chunk_size/3 - 1;
        temp_rect.h = game.data.chunk_size/3 - 1;
        SDL_SetRenderDrawColor(renderer, 231, 134, 34, 255 );
        SDL_RenderFillRect(renderer, &temp_rect );

        // mouse chunk
        temp_rect.x = game.user.mouse_chunk[0];
        temp_rect.y = game.user.mouse_chunk[1];
        temp_rect.w = game.data.chunk_size+1;
        temp_rect.h = game.data.chunk_size+1;
        SDL_SetRenderDrawColor(renderer, 134, 134, 134, 50 );
        SDL_RenderFillRect(renderer, &temp_rect);

        // mouse cursor
        temp_rect.x = game.user.mouse["x"];
        temp_rect.y = game.user.mouse["y"];
        temp_rect.w = game.data.chunk_size/10;
        temp_rect.h = game.data.chunk_size/10;
        SDL_SetRenderDrawColor(renderer, 0, 0, 123, 255 );
        SDL_RenderFillRect(renderer, &temp_rect );

        /*Entity renders*/
        game.update_entities();
        //Only draw visible entities'
        t = 0;
        for(Entity& entity : game.data.visible_entities){

            t++;
            texheight = (game.data.chunk_size) + ((game.data.chunk_size) * .333);
            
            chartexr.x = (entity.local_position.x + (entity.chunkfx * game.data.chunk_size));// - (game.data.size * .7); 
            chartexr.y = (entity.local_position.y + (entity.chunkfy * game.data.chunk_size));// - texheight;
            chartexr.w = game.data.chunk_size; 
            chartexr.h = texheight;
            
            steptexr.x = entity.step * 32; steptexr.y = entity.texAng * 48;
            steptexr.w = 32; steptexr.h = 48; 

            ei = entity.texIndex + 8;
            n = game.data.WorldGen.terrainGeneration(entity.chunk.x, entity.chunk.y);
            if(n == 0){ // && entity.items["boat"] > 0){
                if(entity.speed > 20 && entity.items["fly"] > 0) ei = entity.fast_texIndex;
                else ei = entity.boat_texIndex;
                chartexr.x = (entity.local_position.x + (entity.chunkfx * game.data.chunk_size));// - game.data.chunk_size;
                chartexr.y = (entity.local_position.y + (entity.chunkfy * game.data.chunk_size));// - game.data.chunk_size*3;

                chartexr.w = game.data.chunk_size*2; chartexr.h = texheight*2;
                steptexr.x = 32 * (entity.step % 3); steptexr.y = entity.texAng * 32;
                steptexr.w = 32; steptexr.h = 32; 
                
            }
            else if(entity.speed > 20){
                ei = entity.fast_texIndex;
                chartexr.x = (entity.local_position.x + (entity.chunkfx * game.data.chunk_size));// - game.data.chunk_size;
                chartexr.y = (entity.local_position.y + (entity.chunkfy * game.data.chunk_size));// - game.data.chunk_size*3;
                chartexr.w = game.data.chunk_size*2; chartexr.h = texheight*2;
                steptexr.x = 32; steptexr.y = entity.texAng * 32;
                steptexr.w = 32; steptexr.h = 32; 
            }

            // if(game.data.mouse_entity.ID == entity.ID){
            //     temp_rect.x = entity.local_position.x + (entity.chunkfx * game.data.chunk_size);
            //     temp_rect.y = entity.local_position.y + (entity.chunkfy * game.data.chunk_size);
            //     temp_rect.w = game.data.chunk_size;
            //     temp_rect.h = game.data.chunk_size;
            //     SDL_SetRenderDrawColor(renderer, entity.color.r, entity.color.g, entity.color.b, 255 );
            //     SDL_RenderFillRect(renderer, &temp_rect);
            // // }
            // temp_rect.x =  (entity.chunkfx * game.data.chunk_size);
            // temp_rect.y =  (entity.chunkfy * game.data.chunk_size);
            // temp_rect.w = game.data.chunk_size;
            // temp_rect.h = game.data.chunk_size;
            // SDL_RenderFillRect(renderer, &temp_rect);
            
            SDL_RenderCopy(renderer, game.data.Textures.Textures["characters"][ei].tex, &steptexr, &chartexr);

            if(chartexr.x < game.user.mouse["x"] && game.user.mouse["x"] < chartexr.x + chartexr.w){
                if(chartexr.y < game.user.mouse["y"] && game.user.mouse["y"] < chartexr.y+ chartexr.h){
                    game.data.mouse_entity = entity;
                    if(game.user.mouse_down){
                        if(SDL_GetTicks() > game.user.timer + 3000)
                            send_alert(2); //
                        game.user.timer = SDL_GetTicks();
                        //chartexr.x = entity.local_position.x + (entity.chunkfx * game.data.chunk_size) + 32; 
                        //chartexr.y = entity.local_position.y + (entity.chunkfy * game.data.chunk_size);
                        chartexr.x += game.data.chunk_size/3;
                        chartexr.y += game.data.chunk_size/3;
                        chartexr.w = game.data.chunk_size/3; 
                        chartexr.h = game.data.chunk_size/3;

                        //This seeding sets the entity
                        srand(game.hasher(entity.ID));

                        steptexr.x = (rand() % 9) * 96; steptexr.y = (rand() % 10) * 32;
                        steptexr.w = 32; steptexr.h = 32; 
                        SDL_RenderCopy(renderer, game.data.Textures.Textures["popups"][0].tex, &steptexr, &chartexr);
                        
                    }
                    
                }
            }
        }  

        /*Secondary Tile renders*/
        std::sort(game.data.renderable.begin(), game.data.renderable.end(), [](Position &a, Position &b){ return a.y<b.y; });
        //std::sort(game.data.trees.begin(),game.data.trees.end(), [](Position &a, Position &b){ return a.y<b.y || a.y==b.y && a.noise<b.noise; });
        //std::sort(rocks.begin(),rocks.end(), [](Position &a, Position &b){ return a.y<b.y; });
        //std::sort(game.data.structures.begin(),game.data.structures.end(), [](Building &a, Building &b){ return a.screen_origin[0]<b.screen_origin[1]; });
        for(auto obj : game.data.renderable){
            if(obj.type == 0) {
                srand(floor(obj.noise));
                chartexr.x = 0; 
                chartexr.y = 65 * (rand() % 8);
                chartexr.w = 64; chartexr.h = 64;
                steptexr.x = obj.x - (game.data.chunk_size/2)*3; 
                steptexr.y = obj.y - (game.data.chunk_size*3) - (game.data.chunk_size/2);
                steptexr.w = game.data.chunk_size*4; steptexr.h = game.data.chunk_size*4; 
                SDL_RenderCopy(renderer, game.data.Textures.Textures["tiles"][5].tex, &chartexr, &steptexr);

                if(obj.x < game.user.mouse["x"] && game.user.mouse["x"] < obj.x + game.data.chunk_size){
                    if(obj.y < game.user.mouse["y"] && game.user.mouse["y"] < obj.y + game.data.chunk_size){
                        if(game.user.mouse_down) send_alert(1);

                                        
                        // if(uchunk.x + 1 >= (tree.x + (game.data.chunk_size * 3)) && uchunk.x + 1 <= (tree.x + (game.data.chunk_size * 4))){
                        //     if(uchunk.y + 1 >= (tree.y + (game.data.chunk_size * 6)) && uchunk.y + 1 <= (tree.y  + (game.data.chunk_size * 7))){
                        //         if(game.user.mouse_down) send_alert(0);
                        //     }
                        // }
                    }
                }
            }

            else if(obj.type == 2){
                srand(game.hasher(std::to_string(obj.ix) + std::to_string(obj.iy)));
                std::string bID = game.rstring(10);
                Building b;
                b.global_origin[0] = obj.ix;
                b.global_origin[1] = obj.iy;

                b.screen_origin[0] = obj.x;
                b.screen_origin[1] = obj.y;

                b.roof_index = rand() % 6;
                b.wall_index = rand() % 12;
                b.ID = bID;

                int occupant_count = rand() % 10; //10 is the max occupant count

                //data.structures.push_back(b);

                //draw roof
                for(int r = 0; r < 6; ++r){
                    steptexr.x = 32 * b.roof_index; steptexr.y = 2240;
                    steptexr.w = 32; steptexr.h = 128;
                    temp_rect.x = b.screen_origin[0] + r*game.data.chunk_size; temp_rect.y = b.screen_origin[1];
                    temp_rect.w = game.data.chunk_size; temp_rect.h = game.data.chunk_size * 3; 
                    SDL_RenderCopy(renderer, game.data.Textures.Textures["tiles"][4].tex, &steptexr, &temp_rect);
                }

                //draw front walls
                for(int r = 0; r < 6; ++r){
                    if(r == 0)
                        steptexr.x = 0; 
                    else if(r == 5)
                        steptexr.x = 64;
                    else steptexr.x = 32;


                    steptexr.y = 1407 + (64 * b.wall_index);
                    steptexr.w = 32; steptexr.h = 64;
                    temp_rect.x = b.screen_origin[0] + r*game.data.chunk_size; 
                    temp_rect.y = b.screen_origin[1] + (3*game.data.chunk_size);
                    temp_rect.w = game.data.chunk_size; temp_rect.h = game.data.chunk_size * 3; 
                    SDL_RenderCopy(renderer, game.data.Textures.Textures["tiles"][4].tex, &steptexr, &temp_rect);
                }

                //draw door, window and misc

                //door
                steptexr.x = 224;
                steptexr.y = 1407 + (64 * b.wall_index);
                steptexr.w = 32; steptexr.h = 64;
                temp_rect.x = b.screen_origin[0] + 3*game.data.chunk_size; 
                temp_rect.y = b.screen_origin[1] + (4*game.data.chunk_size);
                temp_rect.w = game.data.chunk_size; temp_rect.h = game.data.chunk_size * 2; 
                SDL_RenderCopy(renderer, game.data.Textures.Textures["tiles"][4].tex, &steptexr, &temp_rect);
                
                //sign
                srand(game.hasher(b.ID));
                int sign_c = (rand() % 8);
                int sign_r = (rand() % 2);
                steptexr.x = 0 + (32 * sign_c);
                steptexr.y = 2624 + (32 * sign_r);
                steptexr.w = 32; steptexr.h = 32;
                temp_rect.x = b.screen_origin[0] + (3*game.data.chunk_size); 
                temp_rect.y = b.screen_origin[1] + (3*game.data.chunk_size);
                temp_rect.w = game.data.chunk_size; temp_rect.h = game.data.chunk_size; 
                SDL_RenderCopy(renderer, game.data.Textures.Textures["tiles"][4].tex, &steptexr, &temp_rect);

                //check if the user's mouse is in the bounds of the structure
                
                bool unlocked = false;
                int room = -1;
                if(sign_c == 3 && sign_r == 1){
                    unlocked = true;
                    room = 1;
                }
                if(sign_c == 7 && sign_r == 1){
                    unlocked = true;
                    room = 2;
                }
                if(sign_c == 1 && sign_r == 1){
                    unlocked = true;
                    room = 3;//
                }
                if(b.screen_origin[0] < game.user.mouse["x"] && game.user.mouse["x"] < b.screen_origin[0] + game.data.chunk_size * 6){
                    if(b.screen_origin[1] < game.user.mouse["y"] && game.user.mouse["y"] < b.screen_origin[1] + game.data.chunk_size * 6){                    
                        //Door position
                        //TODO
                        if(uchunk.x + 1 >= (b.screen_origin[0] + (game.data.chunk_size * 3)) && uchunk.x + 1 <= (b.screen_origin[0] + (game.data.chunk_size * 4))){
                            if(uchunk.y + 1 >= (b.screen_origin[1] + (game.data.chunk_size * 6)) && uchunk.y + 1 <= (b.screen_origin[1] + (game.data.chunk_size * 7))){
                                
                                if(game.user.mouse_down) {
                                    game.user.mouse_down = false;
                                    if(!unlocked) send_alert(0);
                                    else {
                                        if(room == 2) send_alert(5);
                                        else {
                                            if(!game.data.inside) {
                                                if(room == 1) {
                                                    b.type = "book"; 
                                                    send_alert(4);
                                                }
                                                if(room == 3) {
                                                    b.type = "shrek"; 
                                                    send_alert(6);
                                                }
                                                b.start = SDL_GetTicks();
                                                b.step = 0;
                                                game.data.inside = true;
                                                b.user.x = game.data.chunk_size*3;
                                                b.user.y = game.data.chunk_size*5;
                                                game.data.active_interior = b;
                                                game.data.chunk_size = game.data.chunk_sizes[2];

                                                
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        

        for(auto rock : game.data.rocks){
            chartexr.x = 32; chartexr.y = 256;
            chartexr.w = 32; chartexr.h = 32;
            steptexr.x = rock.x; steptexr.y = rock.y;
            steptexr.w = game.data.chunk_size*2; steptexr.h = game.data.chunk_size*2; 
            SDL_RenderCopy(renderer, game.data.Textures.Textures["tiles"][4].tex, &chartexr, &steptexr);
        }

        
    }

    //finally draw everything to the screen
    SDL_RenderPresent(renderer);
    ctx->iteration++;
}

// Begin JS/C bridges

extern "C" {
    int ecount(){
        return game.user.owned_entities.size();
    }
    int get_pos(int type) {
        return 123123;
    }
    const char* get_info(int type){
        const char* retval;
        FastNoise noise;
        switch(type){
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
                retval = game.data.mouse_entity.ID.c_str();
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
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_CreateWindowAndRenderer(game.data.width, game.data.height, 0, &window, &renderer);

    context ctx;
    ctx.renderer = renderer;
    ctx.iteration = 0;

    const int simulate_infinite_loop = 1; // call the function repeatedly
    const int fps = -1; // call the function as fast as the browser wants to render (typically 60fps)
    //emscripten_run_script("var ws = new WebSocket('wss://robauis.me/ws'); ws.onmessage = (e)=>{console.log(e.data);}");
    
    game.data.Textures.renderer = renderer;
    game.data.Textures.loadTextures();

    emscripten_set_main_loop_arg(mainloop, &ctx, fps, simulate_infinite_loop);
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
