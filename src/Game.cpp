#define alphanum "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
#include "Game.h"

//TODO
//This really needs to be a singleton
Game::Game(){
    //Begin defining game settings
    game = GameOBJ{
        .speed = 10,
        .xoffset = 0, .yoffset = 0,
        .width = 1000, .height = 1000,
        .chunk_sizes = std::vector<int>{10, 20, 50, 100, 250, 500},
        .current_chunk_size = 3, .chunk_size = 100, .size = 625,
        .time = SDL_GetTicks(), .MAX_ENTITIES = 10, .default_chk = 3,
        .time_stepx = 0, .time_stepy = 0
    };

    User user {
        .globalx = 1, .globaly = 1,
        .mouse{{"x",0},{"y",0}}, .chunk{0, 0},
        .mouse_chunk{0,0}, .directionx = 1, .directiony = 1
    };
}

//returns the screen x,y coordinate of a specified chunk
Position Game::content(Position& chunk, int bg_render = 0){

    //Unnecessary re-initialization 
    game.dx = (game.xoffset < 0) ? game.chunk_size + game.xoffset : game.xoffset;
    game.dy = (game.yoffset < 0) ? game.chunk_size + game.yoffset : game.yoffset;

    if(user.chunks[0][0] - bg_render <= chunk.x && user.chunks[1][0] + bg_render >= chunk.x && user.chunks[0][1] - bg_render <= chunk.y && user.chunks[2][1] + bg_render >= chunk.y){
        return Position{
            -game.dx + (chunk.x - user.chunks[0][0])*static_cast<int>(game.chunk_size),
            -game.dy + (chunk.y - user.chunks[0][1])*static_cast<int>(game.chunk_size),
            true
        };
    }
    else chunk.visible = false;
    return chunk;
}

//Returns the chunk (x, y) of a specified global position (x, y)
Position Game::getChunkFromCoord(float x, float y) {
    //Unnecessary re-initialization 
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
void Game::update_pos(){
    game.structures.clear();
    //update global position
    //TODO check if the user moves onto a tile they shouldn't
    // i.e Trees, Strucutres, Water Tiles
    //Will need to revert data
    // i.e: globalx/y, directionx/y, x/yoffset
    // zoom_modx/y, xchunk1/2, ychunk1/2, chunks

    float tempx = user.globalx, tempy = user.globaly;
    int temp_directionx = user.directionx, temp_directiony = user.directiony;
    float temp_xoff = game.xoffset, temp_yoff = game.yoffset;
    float temp_zoomx = game.zoom_modx, temp_zoomy = game.zoom_mody;
    int temp_xchunk1 = game.xchunk1, temp_xchunk2 = game.xchunk2;
    int temp_ychunk1 = game.ychunk1, temp_ychunk2 = game.ychunk2;


    if(user.keyState[1]){
        user.globalx+=game.speed * ((user.keyState[5]) ? 15 : 1); //D
        user.directionx = 1;
    }
    if(user.keyState[2]){
        user.globalx-=game.speed * ((user.keyState[5]) ? 15 : 1); //A
        user.directionx = -1;
    }
    if(user.keyState[3]){
        user.globaly+=game.speed * ((user.keyState[5]) ? 15 : 1); //S
        user.directiony = 1;
    }
    if(user.keyState[4]){
        user.globaly-=game.speed * ((user.keyState[5]) ? 15 : 1); //W
        user.directiony = -1;
    } 

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

    //Set the camera bounds
    //This is used to determine what to draw on the screen
    //TODO
    //Can probably be simplified
    //Unnecessary re-initialization 
    game.xchunk1 = floor((user.globalx - (static_cast<float>(game.width)/2)) / static_cast<float>(game.chunk_size)) + game.zoom_modx;
    game.xchunk2 = game.xchunk1 + (static_cast<float>(game.width) / static_cast<float>(game.chunk_size));
    game.ychunk1 = floor((user.globaly - (static_cast<float>(game.height)/2)) / static_cast<float>(game.chunk_size)) + game.zoom_mody;
    game.ychunk2 = game.ychunk1 + (static_cast<float>(game.height) / static_cast<float>(game.chunk_size));

    user.chunks[0][0] = game.xchunk1; user.chunks[0][1] = game.ychunk1;

    user.chunks[1][0] = game.xchunk2; user.chunks[1][1] = game.ychunk1;

    user.chunks[2][0] = game.xchunk1; user.chunks[2][1] = game.ychunk2;

    user.chunks[3][0] = game.xchunk2; user.chunks[3][1] = game.ychunk2;

    //Set the user's chunk
    //Simply calculated by finding the center most chunk, not from the user's global position
    //TODO
    //Set this from the user's global position
    user.chunk[0] = user.chunks[0][0] + floor(static_cast<float>(user.chunks[1][0] - user.chunks[0][0]) / 2);
    user.chunk[1] = user.chunks[0][1] + floor(static_cast<float>(user.chunks[2][1] - user.chunks[0][1]) / 2);

    if(game.WorldGen.terrainGeneration(user.chunk[0], user.chunk[1]) == 0){
        user.globalx = tempx; user.globaly = tempy;
        user.directionx = temp_directionx; user.directiony = temp_directiony;
        game.xoffset = temp_xoff; game.yoffset = temp_yoff;
        game.zoom_modx = temp_zoomx; game.zoom_mody = temp_zoomy;
        game.xchunk1 = temp_xchunk1; game.xchunk2 = temp_xhcunk2;
        game.ychunk1 = temp_ychunk1; game.ychunk2 = temp_ychunk2;

        return;
    }

    game.uchunk = getChunkFromCoord(user.globalx, user.globaly);
    
    //Determines the cuurent mouse chunk
    //TODO
    //Does this really need to be 2 for loops?
    //Change mouse_chunk to a position, so it can be reference with .x and .y
    //user.mouse_chunk[0] = ((ceil(user.mouse["x"]/game.chunk_size)+1) * game.chunk_size) - game.xoffset;
    //user.mouse_chunk[1] = ((ceil(user.mouse["y"]/game.chunk_size)+1) * game.chunk_size) - game.yoffset;
    
    for(int i = 0; i < floor(static_cast<float>(game.width)/static_cast<float>(game.chunk_size)) + 1; i++){
        //Unnecessary re-initialization 
        game.xpos = i*game.chunk_size - game.xoffset;
        
        if(user.mouse["x"] > game.xpos){
            user.mouse_chunk[0] = game.xpos;
        }
    }
    for(int i = 0; i < floor(static_cast<float>(game.height)/static_cast<float>(game.chunk_size)) + 1; i++){    
        //Unnecessary re-initialization 
        game.ypos = i*game.chunk_size - game.yoffset;
        if(user.mouse["y"] > game.ypos){
            user.mouse_chunk[1] = game.ypos;
        }
    }


    //Check for generated content on mouse down
    if(user.mouse_down){
        //Unnecessary re-initialization 
        game.i = user.chunks[0][0] + static_cast<int>(user.mouse_chunk[0]/game.chunk_size);
        game.j = user.chunks[0][1] + static_cast<int>(user.mouse_chunk[1]/game.chunk_size);
        float tile_check = game.WorldGen.terrainGeneration(game.i, game.j);
        if(tile_check == 0){
            if(game.WorldGen.fishGeneration(game.i, game.j, game.time_stepx, game.time_stepy)){
                user.items["fish"] += 1;
                user.mouse_down = false;
            }
        }
        else if(tile_check == 3 || tile_check == 5){
            if(game.WorldGen.treeGeneration(game.i+1, game.j+1)){
                game.WorldGen.ignored_tiles.push_back(std::vector<int>{game.i+1, game.j+1});
                user.items["wood"] += 1;
                user.mouse_down = false;
            }
        }
        else if(tile_check == 2 || tile_check == 6){
            if(game.WorldGen.rockGeneration(game.i-1, game.j-1)){
                user.items["stone"] += 1;
                user.mouse_down = false;
            }
        }

        if(game.WorldGen.doorGeneration(game.i+1, game.j+1)){
            printf("Clicking structure");
        }
    }
}

bool zorder(const Entity &a, const Entity &b){
    return a.position.y < b.position.y;
}
//loop through current entities and update each
//despawn any entites outside of render distance
//update positions for visible entities

void Game::update_entities(){

    //Possible performance hit
    std::sort(game.entities.begin(), game.entities.end(), zorder);
    
    //Unnecessary re-initializations
    unsigned int index;
    float n;
    
    //Possible performance hit
    game.visible_entities.clear();
    
    for(Entity& entity : game.entities){


        //Check if the entity is within n chunks of the user's visible range
        entity.local_position = content(entity.chunk, 5);
        if(entity.local_position.visible){
            game.visible_entities.push_back(entity);

            entity.old_pos = entity.position;

            entity.position.x += (entity.speed * entity.directionx);// + (n * 2);
            entity.position.y += (entity.speed * entity.directiony);// + (n * 2);
            entity.chunk = getChunkFromCoord(entity.position.x, entity.position.y);

            n = (game.noise.GetPerlin((entity.chunk.x), (entity.chunk.y)) - -1) / (1 - -1);
            n = (game.noise.GetPerlinFractal((entity.chunk.x)+pow(n,2), (entity.chunk.y)+pow(n,2)) - -1) / (1 - -1);

            if(entity.ID == game.mouse_entity.ID && user.mouse_down){
                entity.speed = 0;
                if(entity.chunk.x == user.chunk[0] && entity.chunk.x == user.chunk[1]){
                    entity.interacting = true;
                    
                }
            }else entity.speed = entity.temp_speed;

            //If entity hits water and doesn't have a boat, switch directions
            if(n < .45 && entity.items["boat"] < 1){
                entity.position.x = entity.old_pos.x;
                entity.position.y = entity.old_pos.y;
                
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
            }
            if(rand() % 1000 < 5){
                entity.directiony *= -1;
                entity.timey = 0;
                if(entity.directiony > 0) entity.texAng = 0;
                if(entity.directiony < 0) entity.texAng = 3;
            }

            entity.step = (rand() % 1000 < n *50) ? (entity.step+1) : entity.step;
            entity.step%=4;
            entity.timex += .5;
            entity.timey += .5;

        }
        ++index;
    }
}

std::string Game::rstring(size_t length){
    std::string new_key{""};
    for(int i = length; i >= 0; --i){
        new_key += static_cast<std::string>(alphanum).at(rand() % 42);
    }
    return new_key;
}