#pragma once
#include <map>

/// @brief struct containing the position (horizontal and depth) of a submarine
struct SubmarinePos{
    int hor_pos{0}; ///< horizontal position
    int depth{0}; ///< depth
    int aim{0}; ///< aim is used to calculate new depth value by multiplying forward value with aim: forward x aim
};

/// @brief Contains known steer commands of a submarine
enum class SteerCommand : char {
    Up, ///< decreases aim
    Down, ///< increases aim
    Forward ///< increases horizontal position
};

const static std::map<std::string,SteerCommand> STEERING_MAP{
                                                            {"up", SteerCommand::Up}, 
                                                            {"down", SteerCommand::Down},
                                                            {"forward", SteerCommand::Forward}};
