#include "TextureUtils.h"


//TODO
//set file paths and types in a separate file
//read from that file to load textures
std::vector<std::string> shrek_files {
    "Resources/shrek/0.png",
    "Resources/shrek/1.png", "Resources/shrek/2.png", "Resources/shrek/3.png", "Resources/shrek/4.png", "Resources/shrek/5.png", 
    "Resources/shrek/6.png", "Resources/shrek/7.png", "Resources/shrek/8.png", "Resources/shrek/9.png", "Resources/shrek/10.png", "Resources/shrek/11.png", 
    "Resources/shrek/12.png", "Resources/shrek/13.png", "Resources/shrek/14.png", "Resources/shrek/15.png", "Resources/shrek/16.png", "Resources/shrek/17.png", 
    "Resources/shrek/18.png", "Resources/shrek/19.png", "Resources/shrek/20.png", "Resources/shrek/21.png", "Resources/shrek/22.png", "Resources/shrek/23.png", 
    "Resources/shrek/24.png", "Resources/shrek/25.png", "Resources/shrek/26.png", "Resources/shrek/27.png", "Resources/shrek/28.png", "Resources/shrek/29.png", 
    "Resources/shrek/30.png", "Resources/shrek/31.png", "Resources/shrek/32.png", "Resources/shrek/33.png", "Resources/shrek/34.png", "Resources/shrek/35.png", 
    "Resources/shrek/36.png", "Resources/shrek/37.png", "Resources/shrek/38.png", "Resources/shrek/39.png", "Resources/shrek/40.png", "Resources/shrek/41.png", 
    "Resources/shrek/42.png", "Resources/shrek/43.png", "Resources/shrek/44.png", "Resources/shrek/45.png", "Resources/shrek/46.png", "Resources/shrek/47.png", 
    "Resources/shrek/48.png", "Resources/shrek/49.png", "Resources/shrek/50.png", "Resources/shrek/51.png", "Resources/shrek/52.png", "Resources/shrek/53.png", 
    "Resources/shrek/54.png", "Resources/shrek/55.png", "Resources/shrek/56.png", "Resources/shrek/57.png", "Resources/shrek/58.png", "Resources/shrek/59.png", 
    "Resources/shrek/60.png", "Resources/shrek/61.png", "Resources/shrek/62.png", "Resources/shrek/63.png", "Resources/shrek/64.png", "Resources/shrek/65.png", 
    "Resources/shrek/66.png", "Resources/shrek/67.png", "Resources/shrek/68.png", "Resources/shrek/69.png", "Resources/shrek/70.png", "Resources/shrek/71.png", 
    "Resources/shrek/72.png", "Resources/shrek/73.png", "Resources/shrek/74.png", "Resources/shrek/75.png", "Resources/shrek/76.png", "Resources/shrek/77.png", 
    "Resources/shrek/78.png", "Resources/shrek/79.png", "Resources/shrek/80.png", "Resources/shrek/81.png", "Resources/shrek/82.png", "Resources/shrek/83.png", 
    "Resources/shrek/84.png", "Resources/shrek/85.png", "Resources/shrek/86.png", "Resources/shrek/87.png", "Resources/shrek/88.png", "Resources/shrek/89.png", 
    "Resources/shrek/90.png", "Resources/shrek/91.png", "Resources/shrek/92.png", "Resources/shrek/93.png", "Resources/shrek/94.png", "Resources/shrek/95.png", 
    "Resources/shrek/96.png", "Resources/shrek/97.png", "Resources/shrek/98.png", "Resources/shrek/99.png", "Resources/shrek/100.png", "Resources/shrek/101.png", 
    "Resources/shrek/102.png", "Resources/shrek/103.png", "Resources/shrek/104.png", "Resources/shrek/105.png", "Resources/shrek/106.png", "Resources/shrek/107.png", 
    "Resources/shrek/108.png", "Resources/shrek/109.png", "Resources/shrek/110.png", "Resources/shrek/111.png", "Resources/shrek/112.png", "Resources/shrek/113.png", 
    "Resources/shrek/114.png", "Resources/shrek/115.png", "Resources/shrek/116.png", "Resources/shrek/117.png", "Resources/shrek/118.png", "Resources/shrek/119.png", 
    "Resources/shrek/120.png", "Resources/shrek/121.png", "Resources/shrek/122.png", "Resources/shrek/123.png", "Resources/shrek/124.png", "Resources/shrek/125.png", 
    "Resources/shrek/126.png", "Resources/shrek/127.png", "Resources/shrek/128.png", "Resources/shrek/129.png", "Resources/shrek/130.png", "Resources/shrek/131.png", 
    "Resources/shrek/132.png", "Resources/shrek/133.png", "Resources/shrek/134.png", "Resources/shrek/135.png", "Resources/shrek/136.png", "Resources/shrek/137.png",
    "Resources/shrek/138.png"
};

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
    "Resources/tiles/[A]Dirt_pipo.png",
    "Resources/tiles/[A]Grass_pipo.png",
    "Resources/tiles/[A]LongGrass_pipo.png",
    "Resources/tiles/[A]Water_pipo.png",
    "Resources/tiles/[Base]BaseChip_pipo.png",
    "Resources/tiles/trees.png",
    "Resources/tiles/zbeach.png"
    
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

std::vector<std::string> structure_files {
    "Resources/structures/building.png",
};

std::unordered_map<std::string, std::vector<std::string>> texture_types{
    {"characters", char_files}, {"tiles", tile_files},
    {"popups", popup_files}, {"structures", structure_files},
    {"shrek", shrek_files}
};

//,{"items", image_files} {"animals", animal_files}, 
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

