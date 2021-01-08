#ifndef StructureUtils_H
#define StructureUtils_H

#include <unordered_map>
#include <vector>
#include <string>

struct Building {
    int global_origin[2];
    float screen_origin[2];
    int roof_index;
    int wall_index;
    int roof[6][3];
    int front[6][3];  
    int ID;
};

//building 1 = 3x6 roof & 3x6 front wall
class StructureUtils{
    public:
      Building building;
      StructureUtils();
};

#endif