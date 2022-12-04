#include <string>
#include <vector>

#include "../utility.h"
#include "dumbo_octopus_swarm.h"

unsigned int get_total_flashes(const std::string& file_path, const unsigned int steps)
{
    DumboOctopusSwarm<unsigned int, 10> dumbo_swarm{read_2d_vec_from_file<unsigned int>(file_path)};

    return dumbo_swarm.multi_step(steps, false);
}

unsigned int get_synchro_step(const std::string& file_path, const unsigned int steps)
{
    DumboOctopusSwarm<unsigned int, 10> dumbo_swarm{read_2d_vec_from_file<unsigned int>(file_path)};

    return dumbo_swarm.find_synchro_step(false);
}