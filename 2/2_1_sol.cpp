#include <vector>
#include <string>
#include <fstream>

#include "structs_2_1.h"

/**
 * @brief Get the steering commands from input file
 * 
 * @param file_path path to input file
 * @return const std::vector<std::pair<SteerCommand, int>> vector of steering commands
 */
const std::vector<std::pair<SteerCommand, int>> get_steering_input(const std::string& file_path)
{
    std::vector<std::pair<SteerCommand, int>> steer_vec{};
   std::fstream input_file;
   input_file.open(file_path,std::ios::in); //open a file to perform read operation using file object
   if (input_file.is_open()){   //checking whether the file is open
      std::string steer_line;
      while(getline(input_file, steer_line)){  //read data from file object and put it into string.
        size_t pos = steer_line.find( ' ' );
        if (pos == std::string::npos) 
        {
            throw std::runtime_error("Input file %s does not match expected Syntax: 'SteerComand Ind'");
        }
        std::string steer_cmd = steer_line.substr( 0, pos );
        std::string steer_val = steer_line.substr(pos+1, steer_line.length());
        SteerCommand cmd = STEERING_MAP.at(steer_cmd);
        steer_vec.push_back(std::make_pair(cmd,stoi(steer_val)));
      }
      input_file.close();   //close the file object.
   }
   return steer_vec;
}

/**
 * @brief Get the pos after steering a submarine 
 * 
 * @param init_pos initial position of submarine before 
 * @param steering_file_path 
 * @return const SubmarinePos 
 */
const SubmarinePos get_pos_after_steering(SubmarinePos& init_pos, const std::string& steering_file_path)
{
    const std::vector<std::pair<SteerCommand, int>> steer_input = get_steering_input(steering_file_path);

    for (const auto& steer_cmd : steer_input)
    {
        switch(steer_cmd.first) {
            case SteerCommand::Up: 
                init_pos.depth -= steer_cmd.second;
                break;
            case SteerCommand::Down: 
                init_pos.depth += steer_cmd.second;
                break;
            case SteerCommand::Forward: 
                init_pos.hor_pos += steer_cmd.second;
                break;
            default:
                throw std::logic_error("Unknown Enum in steer_cmd!");
        }
    }
    return init_pos;
}

/**
 * @brief Get the solution for task 2 
 * 
 * @param file_path path to input steering file
 * @return const int result of multiplying target depth and horizontal position
 */
const int get_sol_2(const std::string& file_path)
{
    SubmarinePos init_pos{0,0};

    init_pos = get_pos_after_steering(init_pos, file_path);
    return init_pos.depth * init_pos.hor_pos;
}