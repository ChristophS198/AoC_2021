#include <string>

#include "polymer.h"


uint64_t get_element_diff(const std::string& file_path, size_t num_steps)
{
    Polymer polymer(file_path);
    if (num_steps == 40) 
    {
        return polymer.multi_step_40();
    }
    return polymer.multi_step(10);
}

