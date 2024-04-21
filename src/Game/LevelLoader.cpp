#include <sol/sol.hpp>
#include <fstream>
#include <sstream>
#include <string>

#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/KeyboardControlComponent.h"
#include "../Components/CameraFollowComponent.h"
#include "../Components/TextLabelComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/AnimationComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/HealthComponent.h"
#include "../Components/SpriteComponent.h"
#include "./LevelLoader.h"
#include "./Game.h"

LevelLoader::LevelLoader() {
    
}

LevelLoader::~LevelLoader(){
    
}

void LevelLoader::LoadLevel(sol::state& lua, const std::unique_ptr<Registry>& registry, const std::unique_ptr<AssetStore>& assetStore, SDL_Renderer* renderer, int levelNumber) {
       
    sol::protected_function_result script = lua.safe_script_file("./assets/scripts/Level" + std::to_string(levelNumber) + ".lua");
    
    if (!script.valid()){
        sol::error err = script;
        std::string errorMessage = err.what();
        Logger::Err("Error loading the lua script: " + errorMessage); 
        return;
    }
    
    sol::table level = lua["Level"];

    // Add assets to the asset store
    sol::table assets = level["assets"];

    for (int i = 0; i <= assets.size(); i++){
        sol::table asset = assets[i];
        std::string assetType = asset["type"];
        if (assetType == "texture"){
            std::string assetId = asset["id"];
            assetStore -> AddTexture(assetId, asset["file"]);
            Logger::Log("New texture asset loaded to asset store: " + assetId);
        }
        
        if (assetType == "font"){
            std::string assetId = asset["id"];
            assetStore -> AddFont(assetId, asset["file"], asset["font_size"]);
            Logger::Log("New font asset loaded to asset store: " + assetId);
        }
    }

    // Create a 2D tilemap vector
    sol::table tilemap = level["tilemap"];

    std::string tilemapFile = tilemap["map_file"];
    ifstream infile(tilemapFile);
    vector<vector<int>> tileMap;
    string tileMapString;
    
    // Get each line of the map file
    while(getline(infile, tileMapString)){
        // Write each line into a stringstream
        stringstream ss(tileMapString);
        string tile;
        vector<int> row;
        // Get each value from the line, then push it into a 'row' vector
        while(getline(ss, tile, ',')){
            row.push_back(stoi(tile));
        }
        // Push each 'row' vector into the 2D tilemap vector
        tileMap.push_back(row);
    }

    int tileSetWidth = tilemap["width"];
    int tileSize = tilemap["tile_size"];
    double tileScale = tilemap["scale"];
    std::string tilemapTextureAssetId = tilemap["texture_asset_id"]; 
    // Loop over the 2D tileMap vector and create a tile entity for each entry
    for(int i = 0; i < static_cast<int>(tileMap.size()); i++){
        for(int j = 0; j < static_cast<int>(tileMap[i].size()); j++){
            // Define the tile locations based on the indices 
            int tileXPos = j * tileSize * tileScale;
            int tileYPos = i * tileSize * tileScale;
            // Define sprite srcRect based on value
            int tileSetRow = floor(tileMap[i][j] / tileSetWidth);
            int tileSetColumn = (tileMap[i][j] - (tileSetRow * tileSetWidth));
            int srcRectX = tileSize * tileSetColumn;
            int srcRectY = tileSize * tileSetRow;

            Entity tile = registry -> CreateEntity();
            tile.Group("tiles");
            tile.AddComponent<TransformComponent>(glm::vec2(tileXPos, tileYPos), glm::vec2(tileScale, tileScale));
            tile.AddComponent<SpriteComponent>(tilemapTextureAssetId, tileSize, tileSize, 0, false, srcRectX, srcRectY); 
        }
    }
    
    // Set the map width (in pixels) based on the number of rows and columns in the tile map
    Game::mapWidth = tileMap[0].size() * tileSize * tileScale;
    Game::mapHeight = tileMap.size() * tileSize * tileScale;

    // Create entities and add components
    sol::table entities = level["entities"];
    // Loop over entities table
    for (int i = 0; i <= entities.size(); i++){
        // Create Entity
        sol::table entity = entities[i];
        Entity newEntity = registry -> CreateEntity();
        
        // Tag
        sol::optional<std::string> tag = entity["tag"];
        if (tag != sol::nullopt) newEntity.Tag(entity["tag"]);
        
        // Group
        sol::optional<std::string> group = entity["group"];
        if (group != sol::nullopt) newEntity.Group(entity["group"]);

        // Components
        sol::optional<sol::table> hasComponents = entity["components"];
        if (hasComponents != sol::nullopt) {
            sol::table components = entity["components"];           
            for (const auto& key_value_pair : components){
                sol::object key = key_value_pair.first;
                sol::table component = key_value_pair.second;

                std::string componentName = key.as<std::string>();
                
                // Handle different component types 
                if (componentName == "transform") {
                    int positionX = component["position"]["x"];
                    int positionY = component["position"]["y"];
                    glm::vec2 position = glm::vec2(positionX, positionY); 
                    
                    int scaleX = component["scale"]["x"].get_or(1.0);
                    int scaleY = component["scale"]["y"].get_or(1.0);
                    glm::vec2 scale = glm::vec2(scaleX, scaleY);

                    double rotation = component["rotation"].get_or(0.0);

                    newEntity.AddComponent<TransformComponent>(position, scale, rotation); 
                }
                
                if (componentName == "rigidbody") {
                    int velocityX = component["velocity"]["x"].get_or(0.0);
                    int velocityY = component["velocity"]["y"].get_or(0.0);
                    glm::vec2 velocity = glm::vec2(velocityX, velocityY);

                    newEntity.AddComponent<RigidBodyComponent>(velocity);
                }
               
                if (componentName == "sprite") {
                    std::string assetId = component["texture_asset_id"];
                    int width = component["width"];
                    int height = component["height"];
                    int zIndex = component["z_index"].get_or(1);
                    bool isFixed = component["fixed"].get_or(false);
                    int srcRectX = component["src_rect_x"].get_or(0);                    
                    int srcRectY = component["src_rect_y"].get_or(0);

                    newEntity.AddComponent<SpriteComponent>(assetId, width, height, zIndex, isFixed, srcRectX, srcRectY);                                  
                    Logger::Log("Added sprite component to entity " + assetId);
                }
               
                if (componentName == "animation") {
                    // ...handle animation component
                }
               
                if (componentName == "boxcollider") {
                    // ...handle collider component
                }
               
                if (componentName == "health") {
                    // ...handle health component
                }
               
                if (componentName == "projectile_emitter") {
                    // ...handle projectile emitter component
                }
               
                if (componentName == "keyboard_controller") {
                    // ...handle keyboard controller component
                }
               
                if (componentName == "camera_follow") {
                    // ...handle camera component
                }
            }
        }
    }
}
