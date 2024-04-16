//#include "./Game/Game.h"
#include <sol/sol.hpp>
#include <iostream>

void TestLua(){
    sol::state lua;
    lua.open_libraries(sol::lib::base);

    lua.script_file("assets/scripts/myscript.lua");
    
    // Retrieving value of a global variable from a Lua script
    int testVariableInCpp = lua["test_variable"];
    std::cout << "The value of our test lua variable in C++ is " << testVariableInCpp << std::endl;
}

int main(int argc, char* argv[]) {
    //Game game;

    //game.Initialize();
    //game.Run();
    //game.Destroy();    
    
    TestLua();

    return 0;
}
