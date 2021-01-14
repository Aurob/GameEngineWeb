#include "TextureUtils.h"


//TODO
//set file paths and types in a separate file
//read from that file to load textures
std::vector<std::string> shrek_files {
    "Resources/shrek/frame_000_delay-0.02s.png",
    "Resources/shrek/frame_001_delay-0.01s.png","Resources/shrek/frame_002_delay-0.02s.png","Resources/shrek/frame_003_delay-0.02s.png","Resources/shrek/frame_004_delay-0.01s.png","Resources/shrek/frame_005_delay-0.02s.png","Resources/shrek/frame_006_delay-0.02s.png","Resources/shrek/frame_007_delay-0.01s.png","Resources/shrek/frame_008_delay-0.02s.png","Resources/shrek/frame_009_delay-0.02s.png","Resources/shrek/frame_010_delay-0.01s.png",
    "Resources/shrek/frame_011_delay-0.02s.png","Resources/shrek/frame_012_delay-0.02s.png","Resources/shrek/frame_013_delay-0.01s.png","Resources/shrek/frame_014_delay-0.02s.png","Resources/shrek/frame_015_delay-0.02s.png","Resources/shrek/frame_016_delay-0.01s.png","Resources/shrek/frame_017_delay-0.02s.png","Resources/shrek/frame_018_delay-0.02s.png","Resources/shrek/frame_019_delay-0.01s.png","Resources/shrek/frame_020_delay-0.02s.png",
    "Resources/shrek/frame_021_delay-0.02s.png","Resources/shrek/frame_022_delay-0.01s.png","Resources/shrek/frame_023_delay-0.02s.png","Resources/shrek/frame_024_delay-0.02s.png","Resources/shrek/frame_025_delay-0.01s.png","Resources/shrek/frame_026_delay-0.02s.png","Resources/shrek/frame_027_delay-0.02s.png","Resources/shrek/frame_028_delay-0.01s.png","Resources/shrek/frame_029_delay-0.02s.png","Resources/shrek/frame_030_delay-0.02s.png",
    "Resources/shrek/frame_031_delay-0.01s.png","Resources/shrek/frame_032_delay-0.02s.png","Resources/shrek/frame_033_delay-0.02s.png","Resources/shrek/frame_034_delay-0.01s.png","Resources/shrek/frame_035_delay-0.02s.png","Resources/shrek/frame_036_delay-0.02s.png","Resources/shrek/frame_037_delay-0.01s.png","Resources/shrek/frame_038_delay-0.02s.png","Resources/shrek/frame_039_delay-0.02s.png","Resources/shrek/frame_040_delay-0.01s.png",
    "Resources/shrek/frame_041_delay-0.02s.png","Resources/shrek/frame_042_delay-0.02s.png","Resources/shrek/frame_043_delay-0.01s.png","Resources/shrek/frame_044_delay-0.02s.png","Resources/shrek/frame_045_delay-0.02s.png","Resources/shrek/frame_046_delay-0.01s.png","Resources/shrek/frame_047_delay-0.02s.png","Resources/shrek/frame_048_delay-0.02s.png","Resources/shrek/frame_049_delay-0.01s.png","Resources/shrek/frame_050_delay-0.02s.png",
    "Resources/shrek/frame_051_delay-0.02s.png","Resources/shrek/frame_052_delay-0.01s.png","Resources/shrek/frame_053_delay-0.02s.png","Resources/shrek/frame_054_delay-0.02s.png","Resources/shrek/frame_055_delay-0.01s.png","Resources/shrek/frame_056_delay-0.02s.png","Resources/shrek/frame_057_delay-0.02s.png","Resources/shrek/frame_058_delay-0.01s.png","Resources/shrek/frame_059_delay-0.02s.png","Resources/shrek/frame_060_delay-0.02s.png",
    "Resources/shrek/frame_061_delay-0.01s.png","Resources/shrek/frame_062_delay-0.02s.png","Resources/shrek/frame_063_delay-0.02s.png","Resources/shrek/frame_064_delay-0.01s.png","Resources/shrek/frame_065_delay-0.02s.png","Resources/shrek/frame_066_delay-0.02s.png","Resources/shrek/frame_067_delay-0.01s.png","Resources/shrek/frame_068_delay-0.02s.png","Resources/shrek/frame_069_delay-0.02s.png","Resources/shrek/frame_070_delay-0.01s.png",
    "Resources/shrek/frame_071_delay-0.02s.png","Resources/shrek/frame_072_delay-0.02s.png","Resources/shrek/frame_073_delay-0.01s.png","Resources/shrek/frame_074_delay-0.02s.png","Resources/shrek/frame_075_delay-0.02s.png","Resources/shrek/frame_076_delay-0.01s.png","Resources/shrek/frame_077_delay-0.02s.png","Resources/shrek/frame_078_delay-0.02s.png","Resources/shrek/frame_079_delay-0.01s.png","Resources/shrek/frame_080_delay-0.02s.png",
    "Resources/shrek/frame_081_delay-0.02s.png","Resources/shrek/frame_082_delay-0.01s.png","Resources/shrek/frame_083_delay-0.02s.png","Resources/shrek/frame_084_delay-0.02s.png","Resources/shrek/frame_085_delay-0.01s.png","Resources/shrek/frame_086_delay-0.02s.png","Resources/shrek/frame_087_delay-0.02s.png","Resources/shrek/frame_088_delay-0.01s.png","Resources/shrek/frame_089_delay-0.02s.png","Resources/shrek/frame_090_delay-0.02s.png",
    "Resources/shrek/frame_091_delay-0.01s.png","Resources/shrek/frame_092_delay-0.02s.png","Resources/shrek/frame_093_delay-0.02s.png","Resources/shrek/frame_094_delay-0.01s.png","Resources/shrek/frame_095_delay-0.02s.png","Resources/shrek/frame_096_delay-0.02s.png","Resources/shrek/frame_097_delay-0.01s.png","Resources/shrek/frame_098_delay-0.02s.png","Resources/shrek/frame_099_delay-0.02s.png","Resources/shrek/frame_100_delay-0.01s.png",
    "Resources/shrek/frame_101_delay-0.02s.png","Resources/shrek/frame_102_delay-0.02s.png","Resources/shrek/frame_103_delay-0.01s.png","Resources/shrek/frame_104_delay-0.02s.png","Resources/shrek/frame_105_delay-0.02s.png","Resources/shrek/frame_106_delay-0.01s.png","Resources/shrek/frame_107_delay-0.02s.png","Resources/shrek/frame_108_delay-0.02s.png","Resources/shrek/frame_109_delay-0.01s.png","Resources/shrek/frame_110_delay-0.02s.png",
    "Resources/shrek/frame_111_delay-0.02s.png","Resources/shrek/frame_112_delay-0.01s.png","Resources/shrek/frame_113_delay-0.02s.png","Resources/shrek/frame_114_delay-0.02s.png","Resources/shrek/frame_115_delay-0.01s.png","Resources/shrek/frame_116_delay-0.02s.png","Resources/shrek/frame_117_delay-0.02s.png","Resources/shrek/frame_118_delay-0.01s.png","Resources/shrek/frame_119_delay-0.02s.png","Resources/shrek/frame_120_delay-0.02s.png",
    "Resources/shrek/frame_121_delay-0.01s.png","Resources/shrek/frame_122_delay-0.02s.png","Resources/shrek/frame_123_delay-0.02s.png","Resources/shrek/frame_124_delay-0.01s.png","Resources/shrek/frame_125_delay-0.02s.png","Resources/shrek/frame_126_delay-0.02s.png","Resources/shrek/frame_127_delay-0.01s.png","Resources/shrek/frame_128_delay-0.02s.png","Resources/shrek/frame_129_delay-0.02s.png","Resources/shrek/frame_130_delay-0.01s.png"
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

