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
            if(!game.game.inside){
                if(event->wheel.y < 0 && game.game.current_chunk_size > 0) game.game.current_chunk_size--;
                if(event->wheel.y > 0 && game.game.current_chunk_size < 5) game.game.current_chunk_size++;
                
                game.game.chunk_size = game.game.chunk_sizes[game.game.current_chunk_size];

                game.game.size = floor(static_cast<float>(game.game.chunk_size) / 2);
            }

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
            showrick();
            break

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

    //Start to update game content
    game.game.time_stepx += .01;
    game.game.time_stepy += .01;

    if(game.game.inside) game.update_inside();
    else game.update_pos();
    
    //game.game.EntityManager.update();
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

    if(game.game.inside) {

        if((game.game.active_interior.user.x > game.game.chunk_size*3 && game.game.active_interior.user.x < game.game.chunk_size*4) && game.game.active_interior.user.y > game.game.chunk_size*6){
            game.game.inside = false;
        }
        else {
            // small rectangle for the user position
            for (int i = game.user.chunks[0][0]; i < game.user.chunks[1][0] + 1; i++) {
                for (int j = game.user.chunks[0][1]; j < game.user.chunks[3][1] + 1; j++) {
                    tiletexr.x = (i * game.game.chunk_size); tiletexr.y = (j * game.game.chunk_size);
                    tiletexr.w = game.game.chunk_size; tiletexr.h = game.game.chunk_size; 
                    steptexr.x = 0; steptexr.y = 1152;
                    steptexr.w = 32; steptexr.h = 32; 
                    SDL_RenderCopy(renderer, game.game.Textures.Textures["tiles"][4].tex, &steptexr, &tiletexr);
                }
            }
            
            if(game.game.active_interior.items.size() > 0){
                tiletexr.x = game.game.chunk_size * 2.5; tiletexr.y = game.game.chunk_size * 2.5;
                tiletexr.w = game.game.chunk_size; tiletexr.h = game.game.chunk_size*.625; 
                steptexr.x = 64; steptexr.y = 3808;
                steptexr.w = 32; steptexr.h = 20; 
                SDL_RenderCopy(renderer, game.game.Textures.Textures["tiles"][4].tex, &steptexr, &tiletexr);
            }

            if(game.user.mouse_down){
                if(game.game.chunk_size * 2.5 < game.user.mouse["x"] && game.user.mouse["x"] < (game.game.chunk_size * 2.5) + game.game.chunk_size){
                    if(game.game.chunk_size * 2.5 < game.user.mouse["y"] && game.user.mouse["y"] < (game.game.chunk_size * 2.5) + game.game.chunk_size*.625){
                        // small rectangle for the user position
                        
                        send_alert(3);
                    }
                }
            }
            
            temp_rect.x = game.game.active_interior.user.x - game.game.chunk_size/5;
            temp_rect.y = game.game.active_interior.user.y - game.game.chunk_size/5;
            temp_rect.w = game.game.chunk_size/5;
            temp_rect.h = game.game.chunk_size/5;
            SDL_SetRenderDrawColor(renderer, 231, 134, 34, 255 );
            SDL_RenderFillRect(renderer, &temp_rect );

            temp_rect.x = game.game.chunk_size*3;
            temp_rect.y = game.game.chunk_size*6;
            temp_rect.w = game.game.chunk_size;
            temp_rect.h = game.game.chunk_size;
            SDL_SetRenderDrawColor(renderer, 100, 234, 34, 255 );
            SDL_RenderFillRect(renderer, &temp_rect );

            if(game.user.mouse_down) send_alert(3);
        }

    }
    else {
        /*Tile loading*/
        for (int i = game.user.chunks[0][0] - 6; i < game.user.chunks[1][0] + 1; i++) {
            for (int j = game.user.chunks[0][1] - 6; j < game.user.chunks[3][1] + 1; j++) {
                
                srand(hasher(std::to_string(i) + std::to_string(j)));
                //Used to skip drawing of a tile
                //Clicking trees/rocks causes that tile to be skipped
                //TODO
                //Store the reason for skipping the tile so it can be drawn?
                skip = false;
                for(auto structure : ignored_tiles){
                    if(i == structure[0] && j == structure[1]){
                        skip = true;
                        break;
                    }
                }
                //This could be used to limit the size of the world
                //if(i > 100) skip = true;
                if(skip) continue;

                //Get the screen coordinates of the current tile
                Position chunk_position{static_cast<float>(i), static_cast<float>(j)};
                chunk_position = game.content(chunk_position, 6);
                chunk_position.ix = i; chunk_position.iy = j;
                biometex = game.game.WorldGen.terrainGeneration(i, j);
                tiles[biometex].push_back(chunk_position);
                
                //Next determine secondary tile spawns
                //(fish, trees, rocks, etc..)
            
                //load trees on grass tiles
                if(biometex == 4 || biometex == 5){
                    
                    if(game.game.WorldGen.treeGeneration(i, j)){
                        
                        chunk_position.noise = rand() % 10000;
                        trees.push_back(chunk_position);
                    }
                }

                //Rocks spawn on top of stone tiles
                else if(biometex == 6){
                    if(game.game.WorldGen.rockGeneration(i, j)){
                        rocks.push_back(chunk_position);
                    } 
                }

                //Generate fish popups on water only
                else if(biometex == 0){
                    if(game.game.WorldGen.fishGeneration(i, j, game.game.time_stepx, game.game.time_stepy)){
                        fishs.push_back(chunk_position);
                    } 
                }

                //load structures anywhere but water
                if(biometex != 0){
                    if(game.game.WorldGen.doorGeneration(i, j)){
                        std::string bID = game.rstring(10);
                        
                        structures.push_back(chunk_position);
                        Building b;
                        b.global_origin[0] = i;
                        b.global_origin[1] = j;

                        b.screen_origin[0] = chunk_position.x;
                        b.screen_origin[1] = chunk_position.y;

                        b.roof_index = rand() % 6;
                        b.wall_index = rand() % 12;
                        b.ID = bID;

                        int occupant_count = rand() % 10; //10 is the max occupant count

                        //Entity creation
                        // for(unsigned int ii = 0; ii < occupant_count; ++ii){
                        //     float n = game.game.noise.GetPerlinFractal(ii, -ii);
                        //     Entity e {
                        //         .speed = (static_cast<float>((rand() % 30 < 5) ? (rand() % 15) + 15 : rand() % 15)),
                        //         .size = 10, .directionx = 1 - ((rand() % 10 < 5) ? 1 : 0), .directiony = 1 - ((rand() % 10 < 5) ? 1 : 0), 
                        //         .color = SDL_Color{static_cast<Uint8>(rand() % 256), static_cast<Uint8>(rand() % 256), static_cast<Uint8>(rand() % 256)},
                        //         .persist = false, .timex = 0, .timey = 0, .index = ii, .hasTex = true, .texIndex = rand() % MAX_char, .texAng = 0,
                        //         .ID = game.rstring(10), .boat_texIndex = static_cast<unsigned int>(rand() % 6) + 18, 
                        //         .fast_texIndex = (rand() % 6) + (static_cast<int>(game.game.Textures.Textures["characters"].size()) - 6),
                        //         .health = 100.f
                        //     };
                            
                        //     if(e.speed < 20){
                        //         e.fast_texIndex = e.texIndex;
                        //     }
                        //     Position entity_spawn = Position{
                        //         b.screen_origin[0] + static_cast<float>(rand() % (100 - -100) + -100), 
                        //         b.screen_origin[1] + static_cast<float>(rand() % (100 - -100) + -100)
                        //     };

                        //     //Position entity_spawn = Position{static_cast<float>((rand()%100) - fmod(n*400, 100000)*1000), static_cast<float>((rand()%100)*fmod(n*400, 100000)*1000 - fmod(n*400, 10000)*1000)};
                        //     e.chunk = game.getChunkFromCoord(entity_spawn.x, entity_spawn.y);
                        //     n = (game.game.noise.GetPerlin((e.chunk.x), (e.chunk.y)) - -1) / (1 - -1);
                        //     n = (game.game.noise.GetPerlinFractal((e.chunk.x)+pow(n,2), (e.chunk.y)+pow(n,2)) - -1) / (1 - -1);
                        //     if(n < .45){
                        //         e.items["boat"] = 1;
                        //     }
                        //     if(e.fast_texIndex > game.game.Textures.Textures["characters"].size() - 2){
                        //         e.items["fly"] = 1;
                        //     }
                        //     e.temp_speed = e.speed;
                        //     e.position.x = entity_spawn.x;
                        //     e.position.y = entity_spawn.y;
                        //     game.game.entities.push_back(e);

                        //     b.occupants[e.ID] = e;
                        // }

                        b.items["note"] = Position{300, 300};
                        game.game.structures.push_back(b);
                        //Structure spawns are based on a single tile, 
                        //  so we need to check each tile that the structure covers
                        //  and ignore that tile
                        //TODO
                        for(int ii = i; ii < i + 6; ++ii){
                            for(int jj = j; jj < j + 6; ++jj){
                                if(ii != i && jj != j)
                                    ignored_tiles.push_back(std::vector<int>{ii, jj});
                            }
                        }
                    }
                }
            }
        }   
        //concat visible trees to the entities data 
        //game.game.entities.insert(game.game.entities.end(), trees.begin(), trees.end());


        /*Tile renders*/
        for(auto type : tiles){
            
            for(auto tile : type.second){

                tiletexr.x = tile.x; tiletexr.y = tile.y;
                tiletexr.w = game.game.chunk_size; tiletexr.h = game.game.chunk_size; 
                
                //Start by drawing the tile type
                //Will be a terrain tile or city tile
                // if(type.first == 12){
                //     steptexr.x = 32; steptexr.y = 0;
                //     steptexr.w = 32; steptexr.h = 32; 
                // }
                // else if(type.first == 13 || type.first == 8 || type.first == 3){
                //     steptexr.x = 192; steptexr.y = 32;
                //     steptexr.w = 32; steptexr.h = 32;
                // }
                // else{
                //     steptexr.x = 0; steptexr.y = 32;
                //     steptexr.w = 64; steptexr.h = 64; 
                // }
                
                
                int resource_index;
                if(type.first == 0){ //water
                    resource_index = 3;

                    steptexr.x = 192 + (256 * (static_cast<int>(game.game.time_stepx*5)%6));
                    steptexr.y = 1376;

                    int left_tile = game.game.WorldGen.terrainGeneration(tile.ix - 1, tile.iy);
                    int right_tile = game.game.WorldGen.terrainGeneration(tile.ix + 1, tile.iy);
                    int up_tile = game.game.WorldGen.terrainGeneration(tile.ix, tile.iy - 1);
                    int down_tile = game.game.WorldGen.terrainGeneration(tile.ix, tile.iy + 1);
                    int down_left_tile = game.game.WorldGen.terrainGeneration(tile.ix - 1, tile.iy + 1);
                    int up_left_tile = game.game.WorldGen.terrainGeneration(tile.ix - 1, tile.iy - 1);
                    int down_right_tile = game.game.WorldGen.terrainGeneration(tile.ix + 1, tile.iy + 1);
                    int up_right_tile = game.game.WorldGen.terrainGeneration(tile.ix + 1, tile.iy - 1);

                    if(left_tile == 0 && down_tile == 0 && down_left_tile != 0){
                        //draw a sand tile, then draw the water tile on top
                        steptexr.x = 192; steptexr.y = 224;
                        steptexr.w = 32; steptexr.h = 32;
                        SDL_RenderCopy(renderer, game.game.Textures.Textures["tiles"][0].tex, &steptexr, &tiletexr);
                        //
                        steptexr.x = 160 + (256 * (static_cast<int>(game.game.time_stepx*10)%6));
                        steptexr.y = 1440;
                    }
                    else if(left_tile == 0 && up_tile == 0 && up_left_tile != 0){
                        //draw a sand tile, then draw the water tile on top
                        steptexr.x = 192; steptexr.y = 224;
                        steptexr.w = 32; steptexr.h = 32;
                        SDL_RenderCopy(renderer, game.game.Textures.Textures["tiles"][0].tex, &steptexr, &tiletexr);
                        
                        steptexr.x = 160 + (256 * (static_cast<int>(game.game.time_stepx*10)%6));
                        steptexr.y = 1472;
                    }

                    else if(right_tile == 0 && down_tile == 0 && down_right_tile != 0){
                        //draw a sand tile, then draw the water tile on top
                        steptexr.x = 192; steptexr.y = 224;
                        steptexr.w = 32; steptexr.h = 32;
                        SDL_RenderCopy(renderer, game.game.Textures.Textures["tiles"][0].tex, &steptexr, &tiletexr);
                        //
                        steptexr.x = 128 + (256 * (static_cast<int>(game.game.time_stepx*10)%6));
                        steptexr.y = 1440;
                    }
                    else if(right_tile == 0 && up_tile == 0 && up_right_tile != 0){
                        //draw a sand tile, then draw the water tile on top
                        steptexr.x = 192; steptexr.y = 224;
                        steptexr.w = 32; steptexr.h = 32;
                        SDL_RenderCopy(renderer, game.game.Textures.Textures["tiles"][0].tex, &steptexr, &tiletexr);
                        //
                        steptexr.x = 128 + (256 * (static_cast<int>(game.game.time_stepx*10)%6));
                        steptexr.y = 1472;
                    }


                    else if(left_tile == 0 && up_tile != 0 && right_tile == 0){
                        //draw a sand tile, then draw the water tile on top
                        steptexr.x = 192; steptexr.y = 224;
                        steptexr.w = 32; steptexr.h = 32;
                        SDL_RenderCopy(renderer, game.game.Textures.Textures["tiles"][0].tex, &steptexr, &tiletexr);
                        
                        steptexr.x = 192 + (256 * (static_cast<int>(game.game.time_stepx*10)%6));
                        steptexr.y = 1344;
                    }
                    else if(left_tile == 0 && down_tile != 0 && right_tile == 0){
                        //draw a sand tile, then draw the water tile on top
                        steptexr.x = 192; steptexr.y = 224;
                        steptexr.w = 32; steptexr.h = 32;
                        SDL_RenderCopy(renderer, game.game.Textures.Textures["tiles"][0].tex, &steptexr, &tiletexr);
                        //
                        steptexr.x = 192 + (256 * (static_cast<int>(game.game.time_stepx*10)%6));
                        steptexr.y = 1408;
                    }

                    else if(right_tile != 0){
                        
                        //draw a sand tile, then draw the water tile on top
                        steptexr.x = 192; steptexr.y = 224;
                        steptexr.w = 32; steptexr.h = 32;
                        SDL_RenderCopy(renderer, game.game.Textures.Textures["tiles"][0].tex, &steptexr, &tiletexr);
                        //

                        steptexr.x = 224 + (256 * (static_cast<int>(game.game.time_stepx*10)%6));
                        steptexr.y = 1376;

                        if(up_tile != 0){
                            steptexr.y = 1344;
                        }
                        else if(down_tile != 0){
                            steptexr.y = 1408;
                        }
                    }

                    else if(left_tile != 0){
                        
                        //draw a sand tile, then draw the water tile on top
                        steptexr.x = 192; steptexr.y = 224;
                        steptexr.w = 32; steptexr.h = 32;
                        SDL_RenderCopy(renderer, game.game.Textures.Textures["tiles"][0].tex, &steptexr, &tiletexr);
                        //

                        steptexr.x = 160 + (256 * (static_cast<int>(game.game.time_stepx)%6));
                        steptexr.y = 1376;

                        if(up_tile != 0){
                            steptexr.y = 1344;
                        }
                        else if(down_tile != 0){
                            steptexr.y = 1408;
                        }
                    }
                    //TODO
                    //use noise for smoother water transitions.
                    
                    steptexr.w = 32; steptexr.h = 32;
                }
                else if(type.first == 1){ //sand
                        resource_index = 0;
                        steptexr.x = 192; steptexr.y = 224;
                        steptexr.w = 32; steptexr.h = 32;
                }
                else if(type.first == 2){ //dark sand
                        resource_index = 1;
                        steptexr.x = 192; steptexr.y = 32;
                        steptexr.w = 32; steptexr.h = 32;
                }
                else if(type.first == 3){ //dirt
                        resource_index = 0;
                        steptexr.x = 192; steptexr.y = 32;
                        steptexr.w = 32; steptexr.h = 32;
                }
                else if(type.first == 4){ //dark grass
                        resource_index = 1;
                        steptexr.x = 192; steptexr.y = 1184;
                        steptexr.w = 32; steptexr.h = 32;
                }
                else if(type.first == 5){ //grass
                        resource_index = 1;
                        steptexr.x = 192; steptexr.y = 800;
                        steptexr.w = 32; steptexr.h = 32;
                }
                else if(type.first == 6){ //stone
                        resource_index = 0;
                        steptexr.x = 192; steptexr.y = 416;
                        steptexr.w = 32; steptexr.h = 32;
                }

                SDL_RenderCopy(renderer, game.game.Textures.Textures["tiles"][resource_index].tex, &steptexr, &tiletexr);
            }
        }

        for(auto fish : fishs){
            tiletexr.x = fish.x + (game.game.chunk_size/1.5)/3; tiletexr.y = (fish.y) + (game.game.chunk_size/1.5)/4;
            tiletexr.w = game.game.chunk_size/1.5; tiletexr.h = game.game.chunk_size/1.5; 
            steptexr.x = 96 + ((static_cast<int>(game.game.time_stepx/100)) %3)*32; steptexr.y = 192;
            steptexr.w = 32; steptexr.h = 32; 
            SDL_RenderCopy(renderer, game.game.Textures.Textures["popups"][0].tex, &steptexr, &tiletexr);
        }
        
        /*User renders*/
        // fill the chunk the user is currently in
        temp_rect.x = uchunk.x + 1;
        temp_rect.y = uchunk.y + 1;
        temp_rect.w = game.game.chunk_size - 1;
        temp_rect.h = game.game.chunk_size - 1;
        SDL_SetRenderDrawColor(renderer, 134, 134, 134, 128 );
        SDL_RenderFillRect(renderer, &temp_rect );

        // small rectangle for the user position
        temp_rect.x = game.game.width/2 - ((game.game.chunk_size/5)/2);// - (game.game.size * .7); 
        temp_rect.y = game.game.height/2;// - texheight;
        temp_rect.w = game.game.chunk_size/3 - 1;
        temp_rect.h = game.game.chunk_size/3 - 1;
        SDL_SetRenderDrawColor(renderer, 231, 134, 34, 255 );
        SDL_RenderFillRect(renderer, &temp_rect );

        // mouse chunk
        temp_rect.x = game.user.mouse_chunk[0];
        temp_rect.y = game.user.mouse_chunk[1];
        temp_rect.w = game.game.chunk_size+1;
        temp_rect.h = game.game.chunk_size+1;
        SDL_SetRenderDrawColor(renderer, 134, 134, 134, 50 );
        SDL_RenderFillRect(renderer, &temp_rect);

        // mouse cursor
        temp_rect.x = game.user.mouse["x"];
        temp_rect.y = game.user.mouse["y"];
        temp_rect.w = game.game.chunk_size/10;
        temp_rect.h = game.game.chunk_size/10;
        SDL_SetRenderDrawColor(renderer, 0, 0, 123, 255 );
        SDL_RenderFillRect(renderer, &temp_rect );

        /*Entity renders*/
        game.update_entities();
        //Only draw visible entities'
        t = 0;
        for(Entity& entity : game.game.visible_entities){

            t++;
            texheight = (game.game.chunk_size) + ((game.game.chunk_size) * .333);
            
            chartexr.x = (entity.local_position.x + (entity.chunkfx * game.game.chunk_size));// - (game.game.size * .7); 
            chartexr.y = (entity.local_position.y + (entity.chunkfy * game.game.chunk_size));// - texheight;
            chartexr.w = game.game.chunk_size; 
            chartexr.h = texheight;
            
            steptexr.x = entity.step * 32; steptexr.y = entity.texAng * 48;
            steptexr.w = 32; steptexr.h = 48; 

            ei = entity.texIndex + 8;
            n = game.game.WorldGen.terrainGeneration(entity.chunk.x, entity.chunk.y);
            if(n == 0){ // && entity.items["boat"] > 0){
                if(entity.speed > 20 && entity.items["fly"] > 0) ei = entity.fast_texIndex;
                else ei = entity.boat_texIndex;
                chartexr.x = (entity.local_position.x + (entity.chunkfx * game.game.chunk_size));// - game.game.chunk_size;
                chartexr.y = (entity.local_position.y + (entity.chunkfy * game.game.chunk_size));// - game.game.chunk_size*3;

                chartexr.w = game.game.chunk_size*2; chartexr.h = texheight*2;
                steptexr.x = 32 * (entity.step % 3); steptexr.y = entity.texAng * 32;
                steptexr.w = 32; steptexr.h = 32; 
                
            }
            else if(entity.speed > 20){
                ei = entity.fast_texIndex;
                chartexr.x = (entity.local_position.x + (entity.chunkfx * game.game.chunk_size));// - game.game.chunk_size;
                chartexr.y = (entity.local_position.y + (entity.chunkfy * game.game.chunk_size));// - game.game.chunk_size*3;
                chartexr.w = game.game.chunk_size*2; chartexr.h = texheight*2;
                steptexr.x = 32; steptexr.y = entity.texAng * 32;
                steptexr.w = 32; steptexr.h = 32; 
            }

            // if(game.game.mouse_entity.ID == entity.ID){
            //     temp_rect.x = entity.local_position.x + (entity.chunkfx * game.game.chunk_size);
            //     temp_rect.y = entity.local_position.y + (entity.chunkfy * game.game.chunk_size);
            //     temp_rect.w = game.game.chunk_size;
            //     temp_rect.h = game.game.chunk_size;
            //     SDL_SetRenderDrawColor(renderer, entity.color.r, entity.color.g, entity.color.b, 255 );
            //     SDL_RenderFillRect(renderer, &temp_rect);
            // // }
            // temp_rect.x =  (entity.chunkfx * game.game.chunk_size);
            // temp_rect.y =  (entity.chunkfy * game.game.chunk_size);
            // temp_rect.w = game.game.chunk_size;
            // temp_rect.h = game.game.chunk_size;
            // SDL_RenderFillRect(renderer, &temp_rect);
            
            SDL_RenderCopy(renderer, game.game.Textures.Textures["characters"][ei].tex, &steptexr, &chartexr);

            if(chartexr.x < game.user.mouse["x"] && game.user.mouse["x"] < chartexr.x + chartexr.w){
                if(chartexr.y < game.user.mouse["y"] && game.user.mouse["y"] < chartexr.y+ chartexr.h){
                    game.game.mouse_entity = entity;
                    if(game.user.mouse_down){
                        if(SDL_GetTicks() > game.user.timer + 3000)
                            send_alert(2); //
                        game.user.timer = SDL_GetTicks();
                        //chartexr.x = entity.local_position.x + (entity.chunkfx * game.game.chunk_size) + 32; 
                        //chartexr.y = entity.local_position.y + (entity.chunkfy * game.game.chunk_size);
                        chartexr.x += game.game.chunk_size/3;
                        chartexr.y += game.game.chunk_size/3;
                        chartexr.w = game.game.chunk_size/3; 
                        chartexr.h = game.game.chunk_size/3;

                        //This seeding sets the entity
                        srand(hasher(entity.ID));

                        steptexr.x = (rand() % 9) * 96; steptexr.y = (rand() % 10) * 32;
                        steptexr.w = 32; steptexr.h = 32; 
                        SDL_RenderCopy(renderer, game.game.Textures.Textures["popups"][0].tex, &steptexr, &chartexr);
                        
                    }
                    
                }
            }
        }  

        //TODO
        //Each of the secondary tiles could be combined into a single vector

        std::sort(trees.begin(),trees.end(), [](Position &a, Position &b){ return a.y<b.y || a.y==b.y && a.noise<b.noise; });
        //std::sort(rocks.begin(),rocks.end(), [](Position &a, Position &b){ return a.y<b.y; });
        std::sort(game.game.structures.begin(),game.game.structures.end(), [](Building &a, Building &b){ return a.screen_origin[0]<b.screen_origin[1]; });

        /*Secondary Tile renders*/
        for(auto tree : trees){
            srand(floor(tree.noise));
            chartexr.x = 0; 
            chartexr.y = 65 * (rand() % 7);
            chartexr.w = 64; chartexr.h = 64;
            steptexr.x = tree.x - (game.game.chunk_size/2)*3; 
            steptexr.y = tree.y - (game.game.chunk_size*3) - (game.game.chunk_size/2);
            steptexr.w = game.game.chunk_size*4; steptexr.h = game.game.chunk_size*4; 
            SDL_RenderCopy(renderer, game.game.Textures.Textures["tiles"][5].tex, &chartexr, &steptexr);

            if(tree.x < game.user.mouse["x"] && game.user.mouse["x"] < tree.x + game.game.chunk_size){
                if(tree.y < game.user.mouse["y"] && game.user.mouse["y"] < tree.y + game.game.chunk_size){
                    if(game.user.mouse_down) send_alert(1);

                                    
                    // if(uchunk.x + 1 >= (tree.x + (game.game.chunk_size * 3)) && uchunk.x + 1 <= (tree.x + (game.game.chunk_size * 4))){
                    //     if(uchunk.y + 1 >= (tree.y + (game.game.chunk_size * 6)) && uchunk.y + 1 <= (tree.y  + (game.game.chunk_size * 7))){
                    //         if(game.user.mouse_down) send_alert(0);
                    //     }
                    // }
                }
            }
        }

        for(auto rock : rocks){
            chartexr.x = 32; chartexr.y = 256;
            chartexr.w = 32; chartexr.h = 32;
            steptexr.x = rock.x; steptexr.y = rock.y;
            steptexr.w = game.game.chunk_size*2; steptexr.h = game.game.chunk_size*2; 
            SDL_RenderCopy(renderer, game.game.Textures.Textures["tiles"][4].tex, &chartexr, &steptexr);
        }

        for(auto p : game.game.structures){
            //draw roof
            for(int r = 0; r < 6; ++r){
                steptexr.x = 32 * p.roof_index; steptexr.y = 2240;
                steptexr.w = 32; steptexr.h = 128;
                temp_rect.x = p.screen_origin[0] + r*game.game.chunk_size; temp_rect.y = p.screen_origin[1];
                temp_rect.w = game.game.chunk_size; temp_rect.h = game.game.chunk_size * 3; 
                SDL_RenderCopy(renderer, game.game.Textures.Textures["tiles"][4].tex, &steptexr, &temp_rect);
            }

            //draw front walls
            for(int r = 0; r < 6; ++r){
                if(r == 0)
                    steptexr.x = 0; 
                else if(r == 5)
                    steptexr.x = 64;
                else steptexr.x = 32;


                steptexr.y = 1407 + (64 * p.wall_index);
                steptexr.w = 32; steptexr.h = 64;
                temp_rect.x = p.screen_origin[0] + r*game.game.chunk_size; temp_rect.y = p.screen_origin[1] + (3*game.game.chunk_size);
                temp_rect.w = game.game.chunk_size; temp_rect.h = game.game.chunk_size * 3; 
                SDL_RenderCopy(renderer, game.game.Textures.Textures["tiles"][4].tex, &steptexr, &temp_rect);
            }

            //draw door, window and misc

            //door
            steptexr.x = 224;
            steptexr.y = 1407 + (64 * p.wall_index);
            steptexr.w = 32; steptexr.h = 64;
            temp_rect.x = p.screen_origin[0] + 3*game.game.chunk_size; temp_rect.y = p.screen_origin[1] + (4*game.game.chunk_size);
            temp_rect.w = game.game.chunk_size; temp_rect.h = game.game.chunk_size * 2; 
            SDL_RenderCopy(renderer, game.game.Textures.Textures["tiles"][4].tex, &steptexr, &temp_rect);
            
            //sign
            srand(hasher(p.ID));
            int sign_c = (rand() % 8);
            int sign_r = (rand() % 2);
            steptexr.x = 0 + (32 * sign_c);
            steptexr.y = 2624 + (32 * sign_r);
            steptexr.w = 32; steptexr.h = 32;
            temp_rect.x = p.screen_origin[0] + 3*game.game.chunk_size; temp_rect.y = p.screen_origin[1] + (3*game.game.chunk_size);
            temp_rect.w = game.game.chunk_size; temp_rect.h = game.game.chunk_size; 
            SDL_RenderCopy(renderer, game.game.Textures.Textures["tiles"][4].tex, &steptexr, &temp_rect);

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
            if(p.screen_origin[0] < game.user.mouse["x"] && game.user.mouse["x"] < p.screen_origin[0] + game.game.chunk_size * 6){
                if(p.screen_origin[1] < game.user.mouse["y"] && game.user.mouse["y"] < p.screen_origin[1] + game.game.chunk_size * 6){                    
                    //Door position
                    //TODO
                    if(uchunk.x + 1 >= (p.screen_origin[0] + (game.game.chunk_size * 3)) && uchunk.x + 1 <= (p.screen_origin[0] + (game.game.chunk_size * 4))){
                        if(uchunk.y + 1 >= (p.screen_origin[1] + (game.game.chunk_size * 6)) && uchunk.y + 1 <= (p.screen_origin[1] + (game.game.chunk_size * 7))){
                            
                            if(game.user.mouse_down) {
                                game.user.mouse_down = false;
                                if(!unlocked) send_alert(0);
                                else {
                                    if(room == 2) send_alert(5);
                                    else if(!game.game.inside) {
                                        send_alert(4);
                                        game.game.inside = true;
                                        p.user.x = game.game.chunk_size*3;
                                        p.user.y = game.game.chunk_size*6;
                                        game.game.active_interior = p;
                                        game.game.chunk_size = game.game.chunk_sizes[2];
                                        
                                    }
                                }
                            }
                        }
                    }
                }
            }
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

    


    emscripten_set_main_loop_arg(mainloop, &ctx, fps, simulate_infinite_loop);
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
