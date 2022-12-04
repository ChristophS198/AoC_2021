#include <string>
#include <fstream>
#include <vector>

#include "../utility.h"
#include "lanternfish_population.h"

// function declarations
const std::vector<unsigned int> get_lanternfish_population(const std::string& file_path);

uint64_t get_population_size(const std::string& file_path, unsigned int spawn_value, unsigned int first_spawn_value, unsigned int days = 80u, bool debug = false)
{
    std::vector<unsigned int> init_population = read_numbers_from_file<unsigned int>(file_path);
    LanternFishPopulation fish_population{init_population, spawn_value, first_spawn_value};
    if (debug)
    {
        std::ofstream out;
        out.open("population.txt");
        fish_population.simulate_multiple_days(days, out, true);
        out.close();
    }
    else 
    {
        fish_population.simulate_multiple_days(days);
    }
    return fish_population.get_population_size();
}




