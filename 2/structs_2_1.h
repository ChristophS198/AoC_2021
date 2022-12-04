#pragma once
#include <map>

/// @brief struct containing the position (horizontal and depth) of a submarine
struct SubmarinePos{
    int hor_pos{0}; ///< horizontal position
    int depth{0}; ///< depth
};

/// @brief Contains known steer commands of a submarine
enum class SteerCommand : char {
    Up,
    Down,
    Forward
};

const static std::map<std::string,SteerCommand> STEERING_MAP{
                                                            {"up", SteerCommand::Up}, 
                                                            {"down", SteerCommand::Down},
                                                            {"forward", SteerCommand::Forward}};
