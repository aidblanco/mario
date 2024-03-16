#include "Game.h"
#include <cstdlib>
#include <ctime>

Game::Game(Gamedata& data, const char* logfile_name)
{
	logfile.open(logfile_name);
	std::srand((unsigned int)std::time(nullptr));
	this->data = data;
}

void Game::logCurrentState()
{
	auto ent_to_char = [](entity ent) -> char {
			switch (ent)
			{
			case entity::Coin:
				return 'c';
				break;
			case entity::Mushroom:
				return 'm';
				break;
			case entity::Goomba:
				return 'g';
				break;
			case entity::Koopa:
				return 'k';
				break;
			case entity::Boss:
				return 'b';
				break;
			case entity::Nothing:
				return 'x';
				break;
			case entity::Warp:
				return 'w';
				break;
			case entity::Hero:
				return 'H';
				break;
			}
			return 'E'; // E for error
		};
	for (int i = 0; i < data.grid_dimension; ++i){
		std::string line;
		for (int j = 0; j < data.grid_dimension; ++j){
			if (mario_coordinates[0] == i && mario_coordinates[1] == j)
				line.push_back('H');
			else
				line.push_back(ent_to_char(array[i][j]));
		}	
		logfile << line << '\n';
	}
	logfile << '\n';
}

Game::entity Game::randoment()
{
	int randval = std::rand() % 100;
	if (randval < data.percent_coins)
		return entity::Coin;
	randval -= 20;
	if (randval < data.percent_goombas)
		return entity::Goomba;
	randval -= 20;
	if (randval < data.percent_koopas)
		return entity::Koopa;
	randval -= 20;
	if (randval < data.percent_mushrooms)
		return entity::Mushroom;
	randval -= 20;
	if (randval < data.percent_nothing)
		return entity::Nothing;
}

void Game::InitLevel(bool final)
{
	// For the sake of brevity and clarity
	int n = data.grid_dimension;

	array = new entity * [n];

	for (int i = 0; i < n; ++i)
	{
		array[i] = new entity[n];
		for (int j = 0; j < n; ++j)
		{
			array[i][j] = randoment();
		}
	}
	// Add boss and warp
	int boss_location;
	int warp_location;

	do{
		boss_location = std::rand() % (n * n);
		warp_location = std::rand() % (n * n);
		if (final) warp_location = -1;
	} while (boss_location == warp_location);

	array[boss_location % n][boss_location / n] = entity::Boss;
	if (!final) array[warp_location % n][warp_location / n] = entity::Warp;

	logCurrentState();
}

// Return value: 0 for continue; 1 for level beaten; -1 for mario's demise
int Game::IncrementTimestep()
{
	logfile << "Mario begins at position (" << mario_coordinates[0] << ", " << mario_coordinates[1] << ") and with power level: " << power_level << '\n';
	int old_position[2];

	int direction = std::rand() % 4;

	// get the entity on the square in the direction mario is to move
	entity& onsquare = array[mario_coordinates[0]][mario_coordinates[1]];

	int returned_state = 0;

// This macro will make the code look cleaner. We cannot use a lambda because we need to be able to break out of the switch case.
#define DAMAGE_TAKEN \
if (power_level == 0) \
	if (data.num_lives == 0) \
		return -1; /* We return -1 to signify that mario has tragically died */\
	else data.num_lives--, enemies_killed = 0; \
else \
	power_level--; \

	// Calculate the behavior based on the entity at the new location
	switch (onsquare)
	{
	case entity::Coin:
		logfile << "Mario collected a coin. One step closer to paying off that student debt!\n";
		num_coins++;
		if (num_coins == 20)
			data.num_lives++, num_coins = 0;
        onsquare = entity::Nothing;
		break;
	case entity::Mushroom:
		logfile << "Mario ate a Mushroom!\n";
		if (power_level < 3)
			power_level++;
        onsquare = entity::Nothing;
		break;
	case entity::Warp:
		logfile << "Mario enters a mysterious green pipe and emerges in a strange yet familiar environment!\n";
		returned_state = 1;
		break;
	case entity::Goomba:
		logfile << "Mario engages a Goomba!\n";
		if (rand() % 5 > 4) // 80% chance
		{
			logfile << "\t--Mario fought a Goomba and won!\n";
			enemies_killed++;
            onsquare = entity::Nothing;
			break;
		}
		else
		{
			logfile << "\t--Mario fought a Goomba and Lost!\n";
			DAMAGE_TAKEN;
		}
		break;
	case entity::Koopa:
		logfile << "Mario engages a Koopa!\n";
		if (rand() % 20 > 6) // 65% chance
		{
			logfile << "\t--Mario fought a Koopa and won!\n";
			enemies_killed++;
            onsquare = entity::Nothing;
			break;
		}
		else
		{
			logfile << "\t--Oh no! Mario fought a Koopa and Lost! \n";
			DAMAGE_TAKEN;
		}
		break;
	case entity::Boss:
		logfile << "Mario engages the area boss!\n";
		while (true)
		{
			if (rand() % 2 == 0)
			{
				logfile << 
					"Mario fought the level boss and won!\n";
				return 1;
			}
			else
			{
				logfile << "Mario fought the level boss and lost!\n";
				if (power_level > 0) // Take 1 damage if power level is 0 otherwise take 2 damage
				{
					DAMAGE_TAKEN;
				}
				DAMAGE_TAKEN;
			}
		}
		break; // redundant
	default: break;// This will also handle the "entity::Nothing" case
	}

#undef DAMAGE_TAKEN

	if (enemies_killed >= 7)
	{
		logfile << "Mario, who studied blood alchemy in college, has finally accrued enough blood of nis enemies to increase his lifeforce! +1 life!\n";
		data.num_lives++;
		enemies_killed -= 7;
	}

	logfile << "At present Mario has " << num_coins << " coins and " << data.num_lives << " extra lives.\n\n";

	logCurrentState();

	// Generate mario's new postion
	logfile << "\tMario will move ";
	if (direction == 0)
	{
		logfile << "Down.\n\n";
		mario_coordinates[0] += 1;
	}
	else if (direction == 1)
	{
		logfile << "Up.\n\n";
		mario_coordinates[0] -= 1;
	}
	else if (direction == 2)
	{
		logfile << "Right.\n\n";
		mario_coordinates[1] += 1;
	}
	else if (direction == 3)
	{
		logfile << "Left.\n\n";
		mario_coordinates[1] -= 1;
	}

	// Normalize Mario's coordinate;
	mario_coordinates[0] = (mario_coordinates[0] + data.grid_dimension) % data.grid_dimension;
	mario_coordinates[1] = (mario_coordinates[1] + data.grid_dimension) % data.grid_dimension;

	return returned_state;
}

bool Game::Run()
{
	all_arrays = new entity * *[data.num_levels];

	// Generate levels:
	for (int i = 0; i < data.num_levels; ++i)
	{
		InitLevel(i == data.num_levels - 1);
		all_arrays[i] = array;
	}

	int new_mario_position = rand() % (data.grid_dimension * data.grid_dimension);
	mario_coordinates[0] = new_mario_position % data.grid_dimension;
	mario_coordinates[1] = new_mario_position % data.grid_dimension;

	// for each level...
	for (int i = 0; i < data.num_levels; ++i)
	{
		array = all_arrays[i];

		int state;
		while (true)
		{
			logfile << "Level " << i << ": \n";
			state = IncrementTimestep();
			if (state)
				break;
		}
		if (state == -1)
		{
			logfile << "Mario's quest ends in defeat. A mighty oak tree grows at the site of his death whose leaves bear a mighty flaming red during autumn.\n";
			return false;
		}
	}

	logfile << "Mario has successfully rescued the princess. He spends the rest of his days enjoying the spoils of the conquests of his younger years in serenity.\n";
	return true;
}
