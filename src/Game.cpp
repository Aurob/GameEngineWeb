#define alphanum "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
#include "Game.h"

//TODO
//This really needs to be a singleton
Game::Game(){
    //Begin defining game settings
    data = GameOBJ{
        .speed = 10,
        .xoffset = 0, .yoffset = 0,
        .width = 1000, .height = 1000,
        .chunk_sizes = std::vector<int>{2, 20, 50, 100, 250, 500},
        .current_chunk_size = 3, .chunk_size = 100, .size = 625,
        .time = SDL_GetTicks(), .MAX_ENTITIES = 10, .default_chk = 3,
        .time_stepx = 0, .time_stepy = 0
    };

    data.WorldGen.rock_noise.SetSeed(time(NULL));
    User user {
        .globalx = 1, .globaly = 1,
        .mouse{{"x",0},{"y",0}}, .chunk{0, 0},
        .mouse_chunk{0,0}, .directionx = 1, .directiony = 1
    };
}

//returns the screen x,y coordinate of a specified chunk
Position Game::content(Position& chunk, int bg_render = 0){

    data.dx = (data.xoffset < 0) ? data.chunk_size + data.xoffset : data.xoffset;
    data.dy = (data.yoffset < 0) ? data.chunk_size + data.yoffset : data.yoffset;

    if(user.chunks[0][0] - bg_render <= chunk.x && user.chunks[1][0] + bg_render >= chunk.x && user.chunks[0][1] - bg_render <= chunk.y && user.chunks[2][1] + bg_render >= chunk.y){
        return Position{
            -data.dx + (chunk.x - user.chunks[0][0])*static_cast<int>(data.chunk_size),
            -data.dy + (chunk.y - user.chunks[0][1])*static_cast<int>(data.chunk_size),
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
    if (data.current_chunk_size <= data.default_chk-2 || data.current_chunk_size == data.default_chk+2 || data.current_chunk_size == data.default_chk) {
        xchunk = floor((x) / static_cast<float>(data.chunk_sizes[data.default_chk]));
        ychunk = floor((y) / static_cast<float>(data.chunk_sizes[data.default_chk]));
    }
    else if(data.current_chunk_size != data.default_chk){
        xchunk = floor(((x) / fmod(static_cast<float>(data.chunk_size), static_cast<float>(data.chunk_sizes[data.default_chk]))) / 2);
        ychunk = floor(((y) / fmod(static_cast<float>(data.chunk_size), static_cast<float>(data.chunk_sizes[data.default_chk]))) / 2);
    }
    
    return Position{ xchunk, ychunk, true };
}

void Game::clean_data(){
    data.structures.clear();
    data.fishs.clear();
    data.tiles.clear();
    data.rocks.clear();
    data.trees.clear();
    data.structures.clear();
    data.ignored_tiles.clear();
    data.tiles.clear();
    data.renderable.clear();
}

//Batch updates game values
void Game::update_pos(){
    clean_data();
    //update global position
    //TODO check if the user moves onto a tile they shouldn't
    // i.e Trees, Strucutres, Water Tiles
    //Will need to revert data
    // i.e: globalx/y, directionx/y, x/yoffset
    // zoom_modx/y, xchunk1/2, ychunk1/2, chunks

    float tempx = user.globalx, tempy = user.globaly;
    int temp_directionx = user.directionx, temp_directiony = user.directiony;
    float temp_xoff = data.xoffset, temp_yoff = data.yoffset;
    float temp_zoomx = data.zoom_modx, temp_zoomy = data.zoom_mody;
    int temp_xchunk1 = data.xchunk1, temp_xchunk2 = data.xchunk2;
    int temp_ychunk1 = data.ychunk1, temp_ychunk2 = data.ychunk2;


    if(user.keyState[1]){
        user.globalx+=data.speed * ((user.keyState[5]) ? 15 : 1); //D
        user.directionx = 1;
    }
    if(user.keyState[2]){
        user.globalx-=data.speed * ((user.keyState[5]) ? 15 : 1); //A
        user.directionx = -1;
    }
    if(user.keyState[3]){
        user.globaly+=data.speed * ((user.keyState[5]) ? 15 : 1); //S
        user.directiony = 1;
    }
    if(user.keyState[4]){
        user.globaly-=data.speed * ((user.keyState[5]) ? ((data.map_mode) ? 150 : 15) : 1); //W
        user.directiony = -1;
    } 

    //update camera offsets
    if(data.current_chunk_size != data.default_chk){
        data.xoffset = (fmod(user.globalx, static_cast<float>(data.chunk_sizes[data.default_chk])) / static_cast<float>(data.chunk_sizes[data.default_chk])) * static_cast<float>(data.chunk_size);
        data.yoffset = (fmod(user.globaly, static_cast<float>(data.chunk_sizes[data.default_chk])) / static_cast<float>(data.chunk_sizes[data.default_chk])) * static_cast<float>(data.chunk_size);

        data.zoom_modx = floor(user.globalx / static_cast<float>(data.chunk_sizes[data.default_chk])) - floor(user.globalx / static_cast<float>(data.chunk_size));
        data.zoom_mody = floor(user.globaly / static_cast<float>(data.chunk_sizes[data.default_chk])) - floor(user.globaly / static_cast<float>(data.chunk_size));
    }
    else{
        data.xoffset = floor(fmod(user.globalx, static_cast<float>(data.chunk_size)));
        data.yoffset = floor(fmod(user.globaly, static_cast<float>(data.chunk_size)));
        data.zoom_modx = 0;
        data.zoom_mody = 0;
    }

    //Set the camera bounds
    //This is used to determine what to draw on the screen
    //TODO
    //Can probably be simplified
    //Unnecessary re-initialization 
    data.xchunk1 = floor((user.globalx - (static_cast<float>(data.width)/2)) / static_cast<float>(data.chunk_size)) + data.zoom_modx;
    data.xchunk2 = data.xchunk1 + (static_cast<float>(data.width) / static_cast<float>(data.chunk_size));
    data.ychunk1 = floor((user.globaly - (static_cast<float>(data.height)/2)) / static_cast<float>(data.chunk_size)) + data.zoom_mody;
    data.ychunk2 = data.ychunk1 + (static_cast<float>(data.height) / static_cast<float>(data.chunk_size));

    user.chunks[0][0] = data.xchunk1; user.chunks[0][1] = data.ychunk1;

    user.chunks[1][0] = data.xchunk2; user.chunks[1][1] = data.ychunk1;

    user.chunks[2][0] = data.xchunk1; user.chunks[2][1] = data.ychunk2;

    user.chunks[3][0] = data.xchunk2; user.chunks[3][1] = data.ychunk2;

    //Set the user's chunk
    //Simply calculated by finding the center most chunk, not from the user's global position
    //TODO
    //Set this from the user's global position
    user.chunk[0] = user.chunks[0][0] + floor(static_cast<float>(user.chunks[1][0] - user.chunks[0][0]) / 2);
    user.chunk[1] = user.chunks[0][1] + floor(static_cast<float>(user.chunks[2][1] - user.chunks[0][1]) / 2);

    int tile = data.WorldGen.terrainGeneration(user.chunk[0], user.chunk[1]);
    if((!user.keyState[5] && (tile == 0 || ((tile == 4 || tile == 5) && 
        data.WorldGen.treeGeneration(user.chunk[0], user.chunk[1])))) ||
        user.chunk[0] > 100 || user.chunk[0] < -100){

        user.globalx = tempx; user.globaly = tempy;
        user.directionx = temp_directionx; user.directiony = temp_directiony;
        data.xoffset = temp_xoff; data.yoffset = temp_yoff;
        data.zoom_modx = temp_zoomx; data.zoom_mody = temp_zoomy;
        data.xchunk1 = temp_xchunk1; data.xchunk2 = temp_xchunk2;
        data.ychunk1 = temp_ychunk1; data.ychunk2 = temp_ychunk2;

        user.chunks[0][0] = data.xchunk1; user.chunks[0][1] = data.ychunk1;

        user.chunks[1][0] = data.xchunk2; user.chunks[1][1] = data.ychunk1;

        user.chunks[2][0] = data.xchunk1; user.chunks[2][1] = data.ychunk2;

        user.chunks[3][0] = data.xchunk2; user.chunks[3][1] = data.ychunk2;

        user.chunk[0] = user.chunks[0][0] + floor(static_cast<float>(user.chunks[1][0] - user.chunks[0][0]) / 2);
        user.chunk[1] = user.chunks[0][1] + floor(static_cast<float>(user.chunks[2][1] - user.chunks[0][1]) / 2);
    }

    data.uchunk = getChunkFromCoord(user.globalx, user.globaly);
    
    //Determines the cuurent mouse chunk
    //TODO
    //Does this really need to be 2 for loops?
    //Change mouse_chunk to a position, so it can be reference with .x and .y
    //user.mouse_chunk[0] = ((ceil(user.mouse["x"]/data.chunk_size)+1) * data.chunk_size) - data.xoffset;
    //user.mouse_chunk[1] = ((ceil(user.mouse["y"]/data.chunk_size)+1) * data.chunk_size) - data.yoffset;
    
    for(int i = 0; i < floor(static_cast<float>(data.width)/static_cast<float>(data.chunk_size)) + 1; i++){
        //Unnecessary re-initialization 
        data.xpos = i*data.chunk_size - data.xoffset;
        
        if(user.mouse["x"] > data.xpos){
            user.mouse_chunk[0] = data.xpos;
        }
    }
    for(int i = 0; i < floor(static_cast<float>(data.height)/static_cast<float>(data.chunk_size)) + 1; i++){    
        //Unnecessary re-initialization 
        data.ypos = i*data.chunk_size - data.yoffset;
        if(user.mouse["y"] > data.ypos){
            user.mouse_chunk[1] = data.ypos;
        }
    }


    //Check for generated content on mouse down
    if(user.mouse_down){
        //Unnecessary re-initialization 
        data.i = user.chunks[0][0] + static_cast<int>(user.mouse_chunk[0]/data.chunk_size);
        data.j = user.chunks[0][1] + static_cast<int>(user.mouse_chunk[1]/data.chunk_size);
        float tile_check = data.WorldGen.terrainGeneration(data.i, data.j);
        if(tile_check == 0){
            if(data.WorldGen.fishGeneration(data.i, data.j, data.time_stepx, data.time_stepy)){
                user.items["fish"] += 1;
                user.mouse_down = false;
            }
        }
        else if(tile_check == 3 || tile_check == 5){
            if(data.WorldGen.treeGeneration(data.i+1, data.j+1)){
                data.WorldGen.ignored_tiles.push_back(std::vector<int>{data.i+1, data.j+1});
                user.items["wood"] += 1;
                user.mouse_down = false;
            }
        }
        else if(tile_check == 2 || tile_check == 6){
            if(data.WorldGen.rockGeneration(data.i-1, data.j-1)){
                user.items["stone"] += 1;
                user.mouse_down = false;
            }
        }

        if(data.WorldGen.doorGeneration(data.i+1, data.j+1)){
            printf("Clicking structure");
        }
    }

    /*Tile loading*/
    bool skip;
    int biometex;
    for (int i = user.chunks[0][0] - 6; i < user.chunks[1][0] + 6; i++) {
        for (int j = user.chunks[0][1] - 6; j < user.chunks[3][1] + 6; j++) {
            //seed srand with the tile position
            srand(hasher(std::to_string(i) + std::to_string(j)));
            
            //Clicking trees/rocks causes that tile to be skipped
            //TODO
            //Store the reason for skipping the tile so it can be drawn?
            skip = false;
            for(auto structure : data.ignored_tiles){
                if(i == structure[0] && j == structure[1]){
                    skip = true;
                    break;
                }
            }
            if(skip) continue;
            SubTexture tile;

            //Get the screen coordinates of the current tile
            Position chunk_position{static_cast<float>(i), static_cast<float>(j)};
            chunk_position = content(chunk_position, 6);
            chunk_position.ix = i; chunk_position.iy = j;
            biometex = data.WorldGen.terrainGeneration(i, j);
            tile.screen.x = chunk_position.x; tile.screen.y = chunk_position.y;
            tile.screen.w = data.chunk_size; tile.screen.h = data.chunk_size;

            //This could be used to limit the size of the world
            if(i >  100 || i < -100) {
                tile.resource_index = 8;
                if(i > 110 || i < -110) {
                    continue;
                }
                else {
                    tile.texture.x = 32 * (rand() % 2); tile.texture.y = 32 * (rand() % 3);
                    tile.texture.w = 32; tile.texture.h = 32;
                }
                data.tiles.push_back(tile);

                continue;
            }

            if(biometex == 0){ //water
                    tile.resource_index = 3;
                    tile.texture.x = 32*6; tile.texture.y = 224;
                    tile.texture.w = 32; tile.texture.h = 32;
            }
            if(biometex == 1){ //sand
                    tile.resource_index = 0;
                    tile.texture.x = 192; tile.texture.y = 224;
                    tile.texture.w = 32; tile.texture.h = 32;
            }
            else if(biometex == 2){ //dark sand
                    tile.resource_index = 1;
                    tile.texture.x = 192; tile.texture.y = 32;
                    tile.texture.w = 32; tile.texture.h = 32;
            }
            else if(biometex == 3){ //dirt
                    tile.resource_index = 0;
                    tile.texture.x = 192; tile.texture.y = 32;
                    tile.texture.w = 32; tile.texture.h = 32;
            }
            else if(biometex == 4){ //dark grass
                    tile.resource_index = 1;
                    tile.texture.x = 192; tile.texture.y = 1184;
                    tile.texture.w = 32; tile.texture.h = 32;
            }
            else if(biometex == 5){ //grass
                    tile.resource_index = 1;
                    tile.texture.x = 192; tile.texture.y = 800;
                    tile.texture.w = 32; tile.texture.h = 32;
            }
            else if(biometex == 6){ //stone
                    tile.resource_index = 0;
                    tile.texture.x = 192; tile.texture.y = 416;
                    tile.texture.w = 32; tile.texture.h = 32;
            }

            data.tiles.push_back(tile);

            //load trees on grass tiles
            if(biometex == 4 || biometex == 5){
                
                if(data.WorldGen.treeGeneration(i, j)){
                    
                    chunk_position.noise = rand() % 10000;
                    chunk_position.type = 0;
                    data.trees.push_back(chunk_position);
                    data.renderable.push_back(chunk_position);
                }
            }

            //Rocks spawn on top of stone tiles
            else if(biometex == 6){
                if(data.WorldGen.rockGeneration(i, j)){
                    data.rocks.push_back(chunk_position);
                } 
            }

            //Generate fish popups on water only
            else if(biometex == 0){
                if(data.WorldGen.fishGeneration(i, j, data.time_stepx, data.time_stepy)){
                    data.fishs.push_back(chunk_position);
                } 
            }

            //load structures anywhere but water
            if(biometex != 0){
                if(data.WorldGen.doorGeneration(i, j)){
                    chunk_position.type = 2;
                    chunk_position.ix = i; chunk_position.iy = j;
                    data.renderable.push_back(chunk_position);
                    //Structure spawns are based on a single tile, 
                    //  so we need to check each tile that the structure covers
                    //  and ignore that tile
                    //TODO
                    for(int ii = i; ii < i + 6; ++ii){
                        for(int jj = j; jj < j + 6; ++jj){
                            if(ii != i && jj != j)
                                data.ignored_tiles.push_back(std::vector<int>{ii, jj});
                        }
                    }
                }
            }
        }
    }
}

bool zorder(const Entity &a, const Entity &b){
    return a.position.y < b.position.y;
}

void Game::update_inside(){
    user.chunks[0][0] = 0; user.chunks[0][1] = 0;

    user.chunks[1][0] = 6; user.chunks[1][1] = 0;

    user.chunks[2][0] = 0; user.chunks[2][1] = 6;

    user.chunks[3][0] = 6; user.chunks[3][1] = 6;

    if(user.keyState[1]){
        data.active_interior.user.x += data.speed/5; //D
        user.directionx = 1;
    }
    if(user.keyState[2]){
        data.active_interior.user.x -= data.speed/5; //A
        user.directionx = -1;
    }
    if(user.keyState[3]){
        data.active_interior.user.y += data.speed/5; //S
        user.directiony = 1;
    }
    if(user.keyState[4]){
        data.active_interior.user.y -= data.speed/5; //W
        user.directiony = -1;
    } 

    Position ichunk = getChunkFromCoord(data.active_interior.user.x, data.active_interior.user.y);
    
    //Determines the cuurent mouse chunk
    //TODO
    //Does this really need to be 2 for loops?
    //Change mouse_chunk to a position, so it can be reference with .x and .y
    //user.mouse_chunk[0] = ((ceil(user.mouse["x"]/data.chunk_size)+1) * data.chunk_size) - data.xoffset;
    //user.mouse_chunk[1] = ((ceil(user.mouse["y"]/data.chunk_size)+1) * data.chunk_size) - data.yoffset;
    
    for(int i = 0; i < 6 + 1; i++){
        //Unnecessary re-initialization 
        data.xpos = i*data.chunk_size;
        
        if(user.mouse["x"] > data.xpos){
            user.mouse_chunk[0] = data.xpos;
        }
    }
    for(int i = 0; i < 6 + 1; i++){    
        //Unnecessary re-initialization 
        data.ypos = i*data.chunk_size;
        if(user.mouse["y"] > data.ypos){
            user.mouse_chunk[1] = data.ypos;
        }
    }

}
//loop through current entities and update each
//despawn any entites outside of render distance
//update positions for visible entities
void Game::update_entities(){

    //Possible performance hit
    std::sort(data.entities.begin(), data.entities.end(), zorder);
    
    //Unnecessary re-initializations
    unsigned int index;
    float n;
    
    //Possible performance hit
    data.visible_entities.clear();
    
    for(Entity& entity : data.entities){


        //Check if the entity is within n chunks of the user's visible range
        entity.local_position = content(entity.chunk, 5);
        if(entity.local_position.visible){
            data.visible_entities.push_back(entity);

            entity.old_pos = entity.position;

            entity.position.x += (entity.speed * entity.directionx);// + (n * 2);
            entity.position.y += (entity.speed * entity.directiony);// + (n * 2);
            entity.chunk = getChunkFromCoord(entity.position.x, entity.position.y);

            n = (data.noise.GetPerlin((entity.chunk.x), (entity.chunk.y)) - -1) / (1 - -1);
            n = (data.noise.GetPerlinFractal((entity.chunk.x)+pow(n,2), (entity.chunk.y)+pow(n,2)) - -1) / (1 - -1);

            if(entity.ID == data.mouse_entity.ID && user.mouse_down){
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

            entity.chunkfx = abs((entity.chunk.x * data.chunk_sizes[3]) - (entity.position.x)) / data.chunk_sizes[3];
            entity.chunkfy = abs((entity.chunk.y * data.chunk_sizes[3]) - (entity.position.y)) / data.chunk_sizes[3];

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

TileEdge Game::get_tileEdges(int x, int y){
    TileEdge edges;
    edges.left_tile = data.WorldGen.terrainGeneration(x - 1, y);
    edges.right_tile = data.WorldGen.terrainGeneration(x + 1, y);
    edges.up_tile = data.WorldGen.terrainGeneration(x, y - 1);
    edges.down_tile = data.WorldGen.terrainGeneration(x, y + 1);
    edges.down_left_tile = data.WorldGen.terrainGeneration(x - 1, y + 1);
    edges.up_left_tile = data.WorldGen.terrainGeneration(x - 1, y - 1);
    edges.down_right_tile = data.WorldGen.terrainGeneration(x + 1, y + 1);
    edges.up_right_tile = data.WorldGen.terrainGeneration(x + 1, y - 1);
    return edges;
}

std::string Game::rstring(size_t length){
    std::string new_key{""};
    for(int i = length; i >= 0; --i){
        new_key += static_cast<std::string>(alphanum).at(rand() % 42);
    }
    return new_key;
}