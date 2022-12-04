#include <string>

#include "../utility.h"

using tCoord = int;

struct Range 
{
    tCoord min{};
    tCoord max{};
};

struct TargetArea
{
    Range x{};
    Range y{};
};

std::uint32_t find_min_x_start(const tCoord x_min);
TargetArea get_target_area(const std::string &file_path);
bool test_start_vel(const Point<tCoord> start_pt, const TargetArea target_area);
Point<tCoord> probe_step(const Point<tCoord> cur_pos, Point<tCoord> &cur_vel);
bool is_in_target_area(Point<tCoord> pos, TargetArea target_area);


// Idea: When the probe is shot upwards we know that it hits location y=0 on its way down
// and has a velocity of -(v_y_start+1). Therefore, the largest step possible is the one
// where the probe hits the target area with minimal y (-110 in our case).
// So based on the knowledge of the smallest y in our target area we can calculate the 
// start velocity in y-direction and the highest trajectory point
int get_highest_traj_pt(const std::string &file_path)
{
    TargetArea target_area = get_target_area(file_path);

    int y_min = target_area.y.min;
    int v_y0{std::abs(y_min)-1};
    // int num_steps{std::abs(y_min)*2};
    int y_max_traj = std::abs(y_min) * (v_y0) * 0.5; // highest point is reached after num_steps/2 = abs(y_min)

    return y_max_traj;
}

std::uint64_t num_distict_trajectories(const std::string &file_path) 
{
    TargetArea target_area = get_target_area(file_path);

    int y_start_min = target_area.y.min; // Minimal y velocity (direct shot to target area)
    int y_start_max = std::abs(target_area.y.min);
    int x_start_min = find_min_x_start(target_area.x.min); // Minimal x velocity so the probe stops (x-dir) immediately after reaching target area
    int x_start_max = target_area.x.max; // Maximal x velocity -> probe reaches target area in one step

    std::uint64_t trajectory_counter{0};
    for (tCoord x = x_start_min; x <= x_start_max; ++x)
    {
        for (tCoord y = y_start_min; y <= y_start_max; ++y)
        {
            if (test_start_vel(Point<tCoord>{x,y}, target_area))
            {
                ++trajectory_counter;
            }
        }
    }
    return trajectory_counter;
}

bool test_start_vel(Point<tCoord> start_vel, TargetArea target_area)
{
    Point<tCoord> cur_pos{0, 0};
    Point<tCoord> cur_vel{start_vel.x, start_vel.y};
    // Check if target area is reached or overshot
    while (!is_in_target_area(cur_pos, target_area))
    {
        if (cur_pos.x > target_area.x.max || cur_pos.y < target_area.y.min)
        {
            return false;
        }
        // Execute step
        cur_pos = probe_step(cur_pos, cur_vel);
    }
    // std::cout << start_vel.x << ", " << start_vel.y << std::endl;
    return true;
}

bool is_in_target_area(Point<tCoord> pos, TargetArea target_area)
{
    if (pos.x >= target_area.x.min && pos.x <= target_area.x.max &&
        pos.y >= target_area.y.min && pos.y <= target_area.y.max)
    {
        return true;
    }
    else
    {
        return false;
    }
}

Point<tCoord> probe_step(const Point<tCoord> cur_pos, Point<tCoord> &cur_vel)
{
    Point<tCoord> new_pos{cur_pos.x, cur_pos.y};
    new_pos.x += cur_vel.x;
    new_pos.y += cur_vel.y--;
    if (cur_vel.x < 0)
    {
        ++cur_vel.x; -- cur_vel.x;
    }
    if (cur_vel.x > 0)
    {
        --cur_vel.x;
    }
    return new_pos;
}

std::uint32_t find_min_x_start(const tCoord x_min)
{
    int step = 1;
    tCoord i{std::abs(x_min)};
    while ((i-=step++) > 0);

    return --step;
}

TargetArea get_target_area(const std::string &file_path)
{
    std::fstream input_file;
    TargetArea target_area{};
    input_file.open(file_path,std::ios::in);
    if (input_file.is_open())
    {
        std::string input_line;
        getline(input_file, input_line); //read data from file object and put it into string.
        target_area.x.min = get_number_after_str<tCoord>(input_line, "x=");
        target_area.x.max = get_number_after_str<tCoord>(input_line, "..");
        size_t y_start = input_line.find("y=", 0u);
        if (y_start == std::string::npos)
        {
            throw std::runtime_error("y-coordinate of target area not found!");
        }
        target_area.y.min = get_number_after_str<tCoord>(input_line, "y=");
        target_area.y.max = get_number_after_str<tCoord>(input_line.substr(y_start, input_line.length()-y_start), "..");

        input_file.close();   //close the file object.
    }
    return target_area;
}