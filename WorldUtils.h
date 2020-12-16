
#ifndef WorldUtils_H
#define WorldUtils_H

#include <unordered_map>
#include <vector>
#include <string>
#include "FastNoise.h"

struct Pos
{
    float x;
    float y;
    bool visible;
};

class WorldUtils {
    public:
        float n;
        FastNoise terrain_noise;
        FastNoise city_noise;
        FastNoise fish_noise;
        FastNoise tree_noise;
        FastNoise rock_noise;

        std::vector<std::vector<int>> ignored_tiles; //make this an int array
        WorldUtils();
        
        int terrainGeneration(int, int);
        int cityGeneration(int, int);
        bool fishGeneration(int, int, float, float);
        bool treeGeneration(int, int);
        bool rockGeneration(int, int);
};

#endif