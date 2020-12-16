#ifndef Game_H
#define Game_H
#include <iostream>
#include "EntityUtils.h"

#define alphanum "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
EntityUtils::EntityUtils(){
    EntityTypes.push_back("Human");
    EntityTypes.push_back("Animal");
    EntityTypes.push_back("Resource");

    std::cout << "done" << std::endl;
}

void EntityUtils::addEntity(std::string type, Entity& entity){
    //Check if the entity is already in the table
    // if(EntityType_tables.find(entity_table) == EntityType_tables.end()){
    //     EntityType_tables[entity_table] = entity;
    // }
    
}

Entity EntityUtils::newEntity(std::string type){

    //Create the new entity and assign it initial values
    Entity e;

    //Assign new ID and the ID hash
    for(int i = 10; i >= 0; --i){
        e.ID += static_cast<std::string>(alphanum).at(rand() % 42);
    }
    e.IDhash = std::hash<std::string>{}(e.ID);
    
    //if the provided type does not exist, 
    //set the entity to have an empty type and push it to the temp vector
    //use this vector to quickly return unused entities
    for(std::string s : EntityTypes){
        if(s == type){
            e.position = Position{1.f,1.f};
            e.directionx = 1;
            e.directiony = 1;
            EntityLookup[e.IDhash] = s;
            EntityType_tables[type].push_back(e);
            

            return e;
        }
    }
    
    //e.type = 0;
    //temp_Entities.push_back(e);


    //seeded generation means duplicate entities can be created
    //srand(static_cast<int>(hashstr(e.ID)));
    //e.color = SDL_Color{static_cast<Uint8>(rand() % 256),  static_cast<Uint8>(rand() % 256), static_cast<Uint8>(rand() % 256)};
    //e.timenow = SDL_GetTicks();

    return e;    
}

bool EntityUtils::destroyEntity(size_t IDhash){
     if(EntityLookup[IDhash].size() == 0){
         return false;
     }
     else{
         std::vector<Entity> table_Entities = EntityType_tables[EntityLookup[IDhash]];
         for(size_t i = 0; i < table_Entities.size(); ++i){
             if(table_Entities[i].IDhash == IDhash){
                 EntityType_tables[EntityLookup[IDhash]].erase(EntityType_tables[EntityLookup[IDhash]].begin() + i);
                 
             }
         }

         return true;
     }
}

void EntityUtils::update(){
    
    for(auto table : EntityType_tables){
        //std::cout << table.second.size() << " Entities for " << table.first << std::endl;
        for(Entity &e : table.second){
            Movement(e);
            //e
        }
    }
}

void EntityUtils::Movement(Entity &entity){
    
    entity.position.x += entity.directionx;
    entity.position.y += entity.directiony;
}

void EntityUtils::Interaction(){
    
}

Entity EntityUtils::getEntity(size_t IDhash){
    for(auto table : EntityType_tables){
        for(Entity& e : table.second){
            if(e.IDhash == IDhash){
                return e;
            }
            
        }
    }
    return Entity{};
}

// int main(){
//     EntityUtils EntityManager;
//     Entity e = EntityManager.newEntity("Animal");
//     std::cout << e.position.x << std::endl;
//     //EntityManager.destroyEntity(e.IDhash);
//     EntityManager.update();
//     std::cout << EntityManager.getEntity(e.IDhash).ID << std::endl;
//     return 0;
// }
#endif