#include "TextureUtils.h"


//TODO
//set file paths and types in a separate file
//read from that file to load textures

std::vector<std::string> image_files{
    "Resources/items/battery.png",
    "Resources/items/binoculars.png",
    "Resources/items/book.png",
    "Resources/items/bottle.png",
    "Resources/items/bunsen.png",
    "Resources/items/camera.png",
    "Resources/items/double_welder.png",
    "Resources/items/drill.png",
    "Resources/items/extinguisher.png",
    "Resources/items/flashlight.png",
    "Resources/items/game.png",
    "Resources/items/gasmask.png",
    "Resources/items/hacksaw.png",
    "Resources/items/hammer.png",
    "Resources/items/icecream.png",
    "Resources/items/knife.png",
    "Resources/items/lantern.png",
    "Resources/items/medkit.png",
    "Resources/items/microscope.png",
    "Resources/items/plant.png",
    "Resources/items/pliers.png",
    "Resources/items/pump.png",
    "Resources/items/radio.png",
    "Resources/items/sample.png",
    "Resources/items/screwdriver.png",
    "Resources/items/soylent.png",
    "Resources/items/spool.png",
    "Resources/items/syringe.png",
    "Resources/items/tablet.png",
    "Resources/items/thermos.png",
    "Resources/items/tileGrass1.png",
    "Resources/items/welder.png",
    "Resources/items/wrench.png"
};

std::vector<std::string> char_files{
    "Resources/character/Alchemist_walk.png",
    "Resources/character/Barmaid_walk.png",
    "Resources/character/Bartender_walk.png",
    "Resources/character/Blacksmith_walk.png",
    "Resources/character/Farmer_walk.png",
    "Resources/character/Fisherman_walk.png",
    "Resources/character/Kid01_walk.png",
    "Resources/character/Kid02_walk.png",
    "Resources/character/Merchant_walk.png",
    "Resources/character/Alchemist_idle.png",
    "Resources/character/Barmaid_idle.png",
    "Resources/character/Bartender_idle.png",
    "Resources/character/Blacksmith_idle.png",
    "Resources/character/Farmer_idle.png",
    "Resources/character/Fisherman_idle.png",
    "Resources/character/Kid01_idle.png",
    "Resources/character/Kid02_idle.png",
    "Resources/character/Merchant_idle.png",
    "Resources/character/pipo-submarine_2.png",
    "Resources/character/pipo-boat_nosail.png",
    "Resources/character/pipo-boat_1.png",
    "Resources/character/pipo-boat_b_1.png",
    "Resources/character/pipo-ship_c_1.png",
    "Resources/character/pipo-ship_d_1.png",
    "Resources/character/pipo-van_1.png",
    "Resources/character/pipo-van_b_1.png",
    "Resources/character/pipo-van_c_1.png",
    "Resources/character/pipo-balloon_1.png",
    "Resources/character/pipo-balloon_b_1.png",
    "Resources/character/pipo-balloon_c_1.png"
};

std::vector<std::string> tile_files {
    "Resources/tiles/[A]Grass_pipo.png",
    "Resources/tiles/[A]Dirt1-Dirt2_pipo.png",
    "Resources/tiles/[A]Dirt1-Dirt3_pipo.png",
    "Resources/tiles/[A]Grass1-Dirt4_pipo.png",
    "Resources/tiles/[A]Grass1-Dirt1_pipo.png",
    "Resources/tiles/[A]Grass1-Dirt2_pipo.png",
    "Resources/tiles/[A]Grass1-Dirt3_pipo.png",
    "Resources/tiles/[A]Grass1-Dirt4_pipo.png",
    "Resources/tiles/[A]Grass1-Grass2_pipo.png",
    "Resources/tiles/[A]Grass1-Grass3_pipo.png",
    "Resources/tiles/[A]Grass1-Grass4_pipo.png",
    "Resources/tiles/[A]LongGrass_pipo.png",
    "Resources/tiles/[A]Water4_pipo.png",
    "Resources/tiles/[A]Water2_pipo.png",
    "Resources/tiles/[Base]BaseChip_pipo.png"
};

std::vector<std::string> animal_files {
    "Resources/animals/pipo-cat(1).png",
    "Resources/animals/pipo-cat(2).png",
    "Resources/animals/pipo-cat(3).png",
    "Resources/animals/pipo-cat(4).png",
    "Resources/animals/pipo-cat(5).png",
    "Resources/animals/pipo-cat(6).png",
    "Resources/animals/pipo-cat(7).png",
    "Resources/animals/pipo-cat(8).png",
    "Resources/animals/pipo-cat(9).png",
    "Resources/animals/pipo-cat(10).png",
    "Resources/animals/pipo-cat(11).png",
    "Resources/animals/pipo-cat(12).png",
    "Resources/animals/pipo-cat(13).png",
    "Resources/animals/pipo-cat(14).png",
    "Resources/animals/pipo-cat(15).png",
    "Resources/animals/pipo-cat(16).png",
    "Resources/animals/pipo-cat(17).png",
};

std::vector<std::string> popup_files {
    "Resources/world_popup/pipo-popupemotes.png",
};

std::unordered_map<std::string, std::vector<std::string>> texture_types{
    {"items", image_files}, {"characters", char_files}, {"tiles", tile_files},
    {"popups", popup_files}, {"animals", animal_files}
};

bool TextureUtils::loadTextures(){
    if(texturesLoaded) return true;
    else{
        SDL_Texture *img = NULL;
        for(auto tex_type : texture_types){
            for(int i = 0; i < tex_type.second.size(); ++i){
                img = IMG_LoadTexture(renderer, tex_type.second[i].c_str());
                
                Textures[tex_type.first].push_back(Texture{tex_type.second[i].c_str(), 0, 0, img});
                SDL_QueryTexture(Textures[tex_type.first][i].tex, NULL, NULL, 
                    &Textures[tex_type.first][i].w, &Textures[tex_type.first][i].h
                );
            }
        }

        if(Textures.size() == texture_types.size()){
            texturesLoaded = true;
            return true;
        }
        else return false;
    }
};

