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
#include "TextureUtils.h"
#include "WorldUtils.h"
#include <iostream>

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
    std::unordered_map<std::string, int> chunk_cache;
    TextureUtils Textures;
    WorldUtils WorldGen;
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
    std::unordered_map<std::string, int> items;
    bool mouse_down;
    int directionx;
    int directiony;
};

bool zorder(const Entity&, const Entity&);
class Game {
    public:
    GameOBJ game;
    User user;
    Game();
    Position content(Position, int);
    Position getChunkFromCoord(float, float);
    void update_pos();
    void update_entities();
    std::string rstring(size_t);
};