#include <iostream>
#include <fstream>
#include <string>

#include "Game.h"

int main(int argc, char* const argv[]) {
    if (argc != 3)
    {
        std::cout << "Program takes 2 arguments\n";
        return -1;
    }
    const char* const filename = argv[1];
    std::ifstream file(filename);
    if (!file)
    {
        std::cout << "File does not exist\n";
        return -1;
    }
    
    auto get_line_as_int = [&file](int* out) {
        std::string line;
        std::getline(file, line);
        *out = std::stoi(line);
    };

    Gamedata gamedata;
    get_line_as_int(&gamedata.num_levels);
    get_line_as_int(&gamedata.grid_dimension);
    get_line_as_int(&gamedata.num_lives);
    get_line_as_int(&gamedata.percent_coins);
    get_line_as_int(&gamedata.percent_nothing);
    get_line_as_int(&gamedata.percent_goombas);
    get_line_as_int(&gamedata.percent_koopas);
    get_line_as_int(&gamedata.percent_mushrooms);

    Game game(gamedata, argv[2]);
    game.Run();
};