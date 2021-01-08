#ifndef EntityUtils_H
#define EntityUtils_H
#include <SDL2/SDL.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <functional>

// struct User
// {
//     float globalx;
//     float globaly;
//     int chunks[4][2];
//     std::unordered_map<std::string, int> mouse;
//     int chunk[2];
//     int mouse_chunk[2];
//     std::unordered_map<int, bool> keyState;
//     std::unordered_map<std::string, Position> clicks;
//     std::unordered_map<std::string, Entity> owned_entities;
//     std::unordered_map<std::string, int> items;
//     bool mouse_down;
// };
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
    size_t IDhash;
    std::unordered_map<std::string, int> items;
    unsigned int boat_texIndex;
    int fast_texIndex;
    float temp_speed;
    std::vector<void (*)()> Properties;
};

struct Animal : Entity
{
};

class EntityUtils {
    
    
    bool init{false};
    public:
        std::vector<std::string> EntityTypes;
        std::vector<std::string> Properties;
        std::vector<Entity> temp_Entities;
        std::vector<Entity> visible_entities;

        std::unordered_map<std::string, std::vector<Entity>> EntityType_tables;
        std::unordered_map<size_t, std::string> EntityLookup;
        
        EntityUtils();
        void addEntity(std::string, Entity&);
        Entity newEntity(std::string);
        Entity getEntity(size_t);
        bool destroyEntity(size_t);

        void Movement(Entity&);
        void Interaction();
    
        void update();
};


#endif