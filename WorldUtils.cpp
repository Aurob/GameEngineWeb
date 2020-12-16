#include "WorldUtils.h"

WorldUtils::WorldUtils(){
    //Set noise specific presets
    city_noise.SetFrequency(.04);
    city_noise.SetFractalOctaves(32);
    city_noise.SetFractalLacunarity(0.0);
    city_noise.SetFractalType(FastNoise::RigidMulti);

    fish_noise.SetFrequency(.004);
    fish_noise.SetFractalOctaves(12);
    fish_noise.SetFractalLacunarity(0.0);
}

int WorldUtils::terrainGeneration(int i, int j){
    //
    n = (terrain_noise.GetPerlin((i), (j)) - -1) / (1 - -1);
    n = (terrain_noise.GetPerlinFractal((i)+pow(n,2), (j)+pow(n,2)) - -1) / (1 - -1);

    if (n < 0.45) return 13; //water
    else if (n < 0.46) return 2;
    else if (n < 0.48) return 1;
    else if (n < 0.50) return 5;
    else if (n < 0.52) return 4;
    else if (n < 0.67) return 8;
    else return 3;

}

int WorldUtils::cityGeneration(int i, int j){
    //
    if(terrain_noise.GetCellular((i)*2, (j)*2) > .85){
        n = (city_noise.GetSimplex((i)*2, (j)*2));
        //Return 2 to indicate that the tile is in a city and it contains other data, (road, building, etc..)
        if(n > .10 && n < .30) return 2;
        //Return 1 to indicate the tile is in a city, but with no additional data
        return 1;
    }
    //Return 0 to indicate that this is not a city tile
    return 0;
}

bool WorldUtils::fishGeneration(int i, int j, float timex, float timey){
    //TODO
    //maybe check if the provided tile is water first

    n = (fish_noise.GetCellular((timex+i)*100, (timey+j)*100) - -1) / (1 - -1);
    return (n < .90 && n > .83);
}

bool WorldUtils::treeGeneration(int i, int j){
    n = (tree_noise.GetCellular((i)*100, (j)*100) - -1) / (1 - -1);
    return (n < .90 && n > .83);
}

bool WorldUtils::rockGeneration(int i, int j){
    n = (rock_noise.GetCellular((i)*100, (j)*100) - -1) / (1 - -1);
    return (n < .90 && n > .83);
}