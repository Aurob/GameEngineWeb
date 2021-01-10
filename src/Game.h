#include <cstdlib>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <algorithm>
#include "FastNoise.h"
#include "TextureUtils.h"
#include "WorldUtils.h"
#include "StructureUtils.h"
#include <iostream>

struct GameOBJ
{
    float speed;
    float xoffset; //make x, y struct +yoffset
    float yoffset;
    int xchunk; //make x, y struct + xchunk
    int ychunk;
    int zoom_modx; //make x, y struct + zoom_mody
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
    float time_stepx; //make x, y struct +time_stepy
    float time_stepy;
    Entity mouse_entity;
    std::unordered_map<std::string, int> chunk_cache;
    TextureUtils Textures;
    WorldUtils WorldGen;

    //re-used variables
    int dx, dy;
    int xchunk1, xchunk2, ychunk1, ychunk2;
    int xpos, ypos;
    int i, j;
    
    Position uchunk;

    
    std::unordered_map<std::string, Building> structures;
    std::vector<Building> visible_structures;
};

struct User
{
    float globalx; //make Position struct +globaly
    float globaly; 
    int chunks[4][2];
    std::unordered_map<std::string, int> mouse; //make Position struct
    int chunk[2]; //make Position struct
    int mouse_chunk[2]; //make Position struct
    std::unordered_map<int, bool> keyState;
    std::unordered_map<std::string, Position> clicks;
    std::unordered_map<std::string, Entity> owned_entities;
    std::unordered_map<std::string, int> items;
    bool mouse_down;
    int directionx; //make x, y struct +directiony
    int directiony;
    float health;
    int timer;
};

bool zorder(const Entity&, const Entity&);


class Game {
    public:
    GameOBJ game;
    User user;
    Game();
    Position content(Position&, int);
    Position getChunkFromCoord(float, float);
    void update_pos();
    void update_entities();
    std::string rstring(size_t);
};