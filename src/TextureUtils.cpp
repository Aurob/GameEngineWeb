#include "TextureUtils.h"


//TODO
//set file paths and types in a separate file
//read from that file to load textures
std::vector<std::string> shrek_files {
    "Resources/shrek/0.gif", "Resources/shrek/1.gif", "Resources/shrek/2.gif", "Resources/shrek/3.gif", "Resources/shrek/4.gif", "Resources/shrek/5.gif", "Resources/shrek/6.gif", 
    "Resources/shrek/7.gif", "Resources/shrek/8.gif", "Resources/shrek/9.gif", "Resources/shrek/10.gif", "Resources/shrek/11.gif", "Resources/shrek/12.gif", 
    "Resources/shrek/13.gif", "Resources/shrek/14.gif", "Resources/shrek/15.gif", "Resources/shrek/16.gif", "Resources/shrek/17.gif", "Resources/shrek/18.gif", 
    "Resources/shrek/19.gif", "Resources/shrek/20.gif", "Resources/shrek/21.gif", "Resources/shrek/22.gif", "Resources/shrek/23.gif", "Resources/shrek/24.gif", 
    "Resources/shrek/25.gif", "Resources/shrek/26.gif", "Resources/shrek/27.gif", "Resources/shrek/28.gif", "Resources/shrek/29.gif", "Resources/shrek/30.gif", 
    "Resources/shrek/31.gif", "Resources/shrek/32.gif", "Resources/shrek/33.gif", "Resources/shrek/34.gif", "Resources/shrek/35.gif", "Resources/shrek/36.gif", 
    "Resources/shrek/37.gif", "Resources/shrek/38.gif", "Resources/shrek/39.gif", "Resources/shrek/40.gif", "Resources/shrek/41.gif", "Resources/shrek/42.gif", 
    "Resources/shrek/43.gif", "Resources/shrek/44.gif", "Resources/shrek/45.gif", "Resources/shrek/46.gif", "Resources/shrek/47.gif", "Resources/shrek/48.gif", 
    "Resources/shrek/49.gif", "Resources/shrek/50.gif", "Resources/shrek/51.gif", "Resources/shrek/52.gif", "Resources/shrek/53.gif", "Resources/shrek/54.gif", 
    "Resources/shrek/55.gif", "Resources/shrek/56.gif", "Resources/shrek/57.gif", "Resources/shrek/58.gif", "Resources/shrek/59.gif", "Resources/shrek/60.gif", 
    "Resources/shrek/61.gif", "Resources/shrek/62.gif", "Resources/shrek/63.gif", "Resources/shrek/64.gif", "Resources/shrek/65.gif", "Resources/shrek/66.gif", 
    "Resources/shrek/67.gif", "Resources/shrek/68.gif", "Resources/shrek/69.gif", "Resources/shrek/70.gif", "Resources/shrek/71.gif", "Resources/shrek/72.gif", 
    "Resources/shrek/73.gif", "Resources/shrek/74.gif", "Resources/shrek/75.gif", "Resources/shrek/76.gif", "Resources/shrek/77.gif", "Resources/shrek/78.gif", 
    "Resources/shrek/79.gif", "Resources/shrek/80.gif", "Resources/shrek/81.gif", "Resources/shrek/82.gif", "Resources/shrek/83.gif", "Resources/shrek/84.gif", 
    "Resources/shrek/85.gif", "Resources/shrek/86.gif", "Resources/shrek/87.gif", "Resources/shrek/88.gif", "Resources/shrek/89.gif", "Resources/shrek/90.gif", 
    "Resources/shrek/91.gif", "Resources/shrek/92.gif", "Resources/shrek/93.gif", "Resources/shrek/94.gif", "Resources/shrek/95.gif", "Resources/shrek/96.gif", 
    "Resources/shrek/97.gif", "Resources/shrek/98.gif", "Resources/shrek/99.gif", "Resources/shrek/100.gif", "Resources/shrek/101.gif", "Resources/shrek/102.gif", 
    "Resources/shrek/103.gif", "Resources/shrek/104.gif", "Resources/shrek/105.gif", "Resources/shrek/106.gif", "Resources/shrek/107.gif", "Resources/shrek/108.gif", 
    "Resources/shrek/109.gif", "Resources/shrek/110.gif", "Resources/shrek/111.gif", "Resources/shrek/112.gif", "Resources/shrek/113.gif", "Resources/shrek/114.gif", 
    "Resources/shrek/115.gif", "Resources/shrek/116.gif", "Resources/shrek/117.gif", "Resources/shrek/118.gif", "Resources/shrek/119.gif", "Resources/shrek/120.gif", 
    "Resources/shrek/121.gif", "Resources/shrek/122.gif", "Resources/shrek/123.gif", "Resources/shrek/124.gif", "Resources/shrek/125.gif", "Resources/shrek/126.gif", 
    "Resources/shrek/127.gif", "Resources/shrek/128.gif", "Resources/shrek/129.gif", "Resources/shrek/130.gif", "Resources/shrek/131.gif", "Resources/shrek/132.gif", 
    "Resources/shrek/133.gif", "Resources/shrek/134.gif", "Resources/shrek/135.gif", "Resources/shrek/136.gif", "Resources/shrek/137.gif", "Resources/shrek/138.gif"
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

