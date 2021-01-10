#ifndef StructureUtils_H
#define StructureUtils_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <unordered_map>
#include <vector>
#include <string>

struct Position
{
    float x;
    float y;
    bool visible;
    float noise;
};

struct Entity
{
    Position position; //global position
    Position chunk; //global chunk
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
    Position local_position;
    Position old_pos;
    float health;
    bool interacting;
};

struct Building {
    int global_origin[2];
    float screen_origin[2];
    int roof_index;
    int wall_index;
    int roof[6][3];
    int front[6][3];  
    std::string ID;

    std::unordered_map<std::string, Entity> occupants;
};

//building 1 = 3x6 roof & 3x6 front wall
class StructureUtils{
    public:
      Building building;
      StructureUtils();
};

#endif