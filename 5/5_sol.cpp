#include <iostream>
#include <fstream>
#include <memory>
#include <string>

#include "../utility.h"
#include "ocean_floor_grid.h"

std::shared_ptr<OceanFloorGrid> get_ocean_floor_grid(const std::string& file_path, const unsigned int);
Line<unsigned int> parse_line(const std::string& line);


/**
 * @brief Get the number of points in the ocean floor grid, where more than x lines overlap
 * 
 * @param file_path path to file containing hydrothermal vent lines
 * @param threshold more than threshold lines must overlap in a single grid cell to be counted 
 * @param init_grid_size initial size of grid
 * @return const unsigned int number of grid cells where more than threshold hydrothermal vent lines overlap
 */
const unsigned int get_num_points_larger_x(const std::string& file_path, const unsigned int threshold, const unsigned int init_grid_size)
{
    std::shared_ptr<OceanFloorGrid> ocean_grid = get_ocean_floor_grid(file_path, init_grid_size);
    return ocean_grid->get_num_points_larger_x(threshold);
}

/**
 * @brief Get the coordinates of hydrothermal vents from input file 
 * 
 * @param file_path path to input file
 * @param init_grid_size initial size of grid
 * @return const std::vector<std::string> vector of strings containing bits ['0','1']
 */
std::shared_ptr<OceanFloorGrid> get_ocean_floor_grid(const std::string& file_path, const unsigned int init_grid_size)
{
    std::shared_ptr<OceanFloorGrid> floor_grid = std::make_shared<OceanFloorGrid>(init_grid_size);
    std::fstream input_file;
    input_file.open(file_path,std::ios::in);
    if (input_file.is_open()){
        std::string input_line;
        while(getline(input_file, input_line)){  
            Line<unsigned int> line = parse_line(input_line);
            floor_grid->add_line(line);
        }
        input_file.close();   
    }
    return floor_grid;
}

/**
 * @brief Parse input string of form 55,23 -> 218,660
 * 
 * @param line 
 * @return Line<unsigned int> 
 */
Line<unsigned int> parse_line(const std::string& line)
{
    Line<unsigned int> line_coord{};
    std::vector<unsigned int> coords{};
    std::vector<char> c_vec;
    for (const auto& c : line)
    {
        if (std::isdigit(static_cast<unsigned char>(c)))
        {
            c_vec.push_back(c);
        }
        else
        {
            if(c_vec.size() > 0)
            {
                coords.push_back(convert_to_num<unsigned int>(c_vec));
                c_vec.clear();
            }
        }
    }
    // Convert and insert last number
    if(c_vec.size() > 0)
    {
        coords.push_back(convert_to_num<unsigned int>(c_vec));
        c_vec.clear();
    }
    if (coords.size() != 4)
    {
        std::string str = std::string("Input '") + line + std::string("' cannot be parsed to 4 numbers!");
        throw std::runtime_error(str);
    }
    else
    {
        line_coord.start.x = coords[0];
        line_coord.start.y = coords[1];
        line_coord.end.x   = coords[2];
        line_coord.end.y   = coords[3];
    }
    return line_coord;
} 

