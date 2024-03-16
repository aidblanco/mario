 #include <iostream>
#include <fstream>

struct Gamedata {
    int num_levels;
    int grid_dimension;
    int num_lives;
    int percent_coins;
    int percent_goombas;
    int percent_nothing;
    int percent_koopas;
    int percent_mushrooms;
};

class Game {
public:
	Game(Gamedata& data, const char* logfile_name);
	bool Run();
private:
	Game() = default;
	Gamedata data;
    std::ofstream logfile;

    int mario_coordinates[2] = {-1, -1};
    
	enum class entity : unsigned char {Coin = 0, Mushroom, Goomba, Koopa, Boss, Nothing, Warp, Hero};

    entity*** all_arrays;
    entity** array;
    int num_coins = 0;
    int enemies_killed;
    int power_level = 0;

    entity randoment();
    void InitLevel(bool final);
    void logCurrentState();
    int IncrementTimestep();
};