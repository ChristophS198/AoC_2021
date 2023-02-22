#include <string>
#include <vector>
#include <array>

#include "../utility.h"

constexpr uint16_t SIZE = 50u; // Grid goes from -SIZE....SIZE
constexpr uint16_t GRID_SIZE{ SIZE*2 + 1 }; // Array ranges from 0 to SIZE*2 + 1

using pCoord = int64_t;
using Grid = std::array<std::array<std::array<uint8_t, GRID_SIZE>, GRID_SIZE>, GRID_SIZE>;

enum class InsType{
    ON,
    OFF,
};

struct Range
{
    pCoord start;
    pCoord end;
};

struct Cuboid
{
    Range x;
    Range y;
    Range z;
};

struct Instruction
{
    InsType type;
    Cuboid cuboid;
};

void print_instructions(std::ostream &out, const std::vector<Instruction> &ins_vec);
std::vector<Instruction> get_instructions(const std::string& file_path);
std::vector<Instruction> trafo_instructions(const std::vector<Instruction> &ins_vec, const pCoord lower_lim, const pCoord upper_lim);
void limit_point(Instruction &ins, const pCoord lower_lim, const pCoord upper_lim);
bool is_in_range(Instruction &ins, const pCoord lower_lim, const pCoord upper_lim);
void execute_instruction(const Instruction &ins, Grid &grid);
uint64_t count_active_cubes(const Grid &grid);
bool do_cuboids_intersect(const Cuboid &c1, const Cuboid &c2);
std::vector<Range> get_not_intersecting_ranges_r1(const Range r1, const Range r2);
std::vector<Cuboid> get_not_intersecting_cuboids(const Cuboid &c1, const Cuboid &c2);
uint64_t count_active_cubes(const std::vector<Cuboid> &c_vec);

std::uint64_t day_22_1(const std::string &file_path)
{
    std::vector<Instruction> ins_vec = get_instructions(file_path);
    ins_vec = trafo_instructions(ins_vec, 0, 100);
    // print_instructions(std::cout, ins_vec);
    Grid grid{};
    for (const auto &ins : ins_vec)
    {
        execute_instruction(ins, grid);
    }

    return count_active_cubes(grid);
}

/*
Idea is to iteratively check all "on" instructions from top to bottom. 
If the current "on"-cube has any grid points (cubes) that have no intersection
with any following "on" or "off" cube they are counted to the sum of active grid points. 
If they intersect with a following "on" cuboid they will be counted later on, if they intersect 
with an "off"-cuboid they will not be counted.
*/
std::uint64_t day_22_2(const std::string &file_path)
{
    uint64_t sum{ 0u };
    std::vector<Instruction> ins_vec = get_instructions(file_path);
    for (size_t i=0; i<ins_vec.size(); ++i)
    {
        if (ins_vec[i].type == InsType::OFF) 
        {
            continue;
        }
        std::vector<Cuboid> not_intersecting_cuboids{ ins_vec[i].cuboid };
        for (size_t j=i+1; j<ins_vec.size(); ++j)
        {
            std::vector<Cuboid> new_not_intersecting_cuboids;

            for (const auto &c : not_intersecting_cuboids)
            {
                auto tmp = get_not_intersecting_cuboids(c, ins_vec[j].cuboid);
                new_not_intersecting_cuboids.insert(new_not_intersecting_cuboids.end(), tmp.begin(), tmp.end());
            }
            not_intersecting_cuboids = new_not_intersecting_cuboids;
        }
        sum += count_active_cubes(not_intersecting_cuboids);
    }
    return sum;
}

/* returns a vector of cuboids of c1 that are not part of c2 
Up to 6 new cuboids are returned 
*/
std::vector<Cuboid> get_not_intersecting_cuboids(const Cuboid &c1, const Cuboid &c2)
{
    std::vector<Cuboid> cuboids;
    if(!do_cuboids_intersect(c1, c2)) 
    {
        cuboids.push_back(c1);
        return cuboids;
    }

    Range cur_x_range = c1.x;
    Range cur_y_range = c1.y;

    std::vector<Range> x_range = get_not_intersecting_ranges_r1(c1.x, c2.x);
    for (const auto &r: x_range)
    {
        Cuboid c{ r, c1.y, c1.z };
        cuboids.push_back(c);
        // reduce size of x range so we do not include those points twice
        if (r.start == cur_x_range.start) cur_x_range.start = r.end+1;
        else cur_x_range.end = r.start-1;
    }
    std::vector<Range> y_range = get_not_intersecting_ranges_r1(c1.y, c2.y);
    for (const auto &r: y_range)
    {
        Cuboid c{ cur_x_range, r, c1.z };
        cuboids.push_back(c);
        // reduce size of y range so we do not include those points twice
        if (r.start == cur_y_range.start) cur_y_range.start = r.end+1;
        else cur_y_range.end = r.start-1;
    }
    std::vector<Range> z_range = get_not_intersecting_ranges_r1(c1.z, c2.z);
    for (const auto &r: z_range)
    {
        Cuboid c{ cur_x_range, cur_y_range, r };
        cuboids.push_back(c);
    }
    return cuboids;
}

// returns a vector of sub-ranges of r1 that are not in range of r2 
std::vector<Range> get_not_intersecting_ranges_r1(const Range r1, const Range r2)
{
    std::vector<Range> ranges;
    if (r1.start < r2.start)
    {
        pCoord start = r1.start;
        pCoord end = std::min(r1.end, r2.start-1);
        ranges.push_back({ start, end });
    }
    if (r2.end < r1.end)
    {
        pCoord start = std::max(r1.start, r2.end+1);
        pCoord end = r1.end;
        ranges.push_back({ start, end });
    }
    return ranges;
}

uint64_t count_active_cubes(const std::vector<Cuboid> &c_vec)
{
    uint64_t sum{ 0u };
    for (const auto &c : c_vec)
    {
        sum += (c.x.end - c.x.start + 1) * (c.y.end - c.y.start + 1) * (c.z.end - c.z.start + 1);
    }
    return sum;
}

// if any of the ranges do not intersect -> no intersection 
bool do_cuboids_intersect(const Cuboid &c1, const Cuboid &c2)
{
    if (c1.x.end < c2.x.start || c1.x.start > c2.x.end) return false;
    if (c1.y.end < c2.y.start || c1.y.start > c2.y.end) return false;
    if (c1.z.end < c2.z.start || c1.z.start > c2.z.end) return false;
    return true;
}

void execute_instruction(const Instruction &ins, Grid &grid)
{
    uint8_t t{ 0u };
    if (InsType::ON == ins.type)
    {
        t = 1u;
    }
    for (auto x=ins.cuboid.x.start; x<=ins.cuboid.x.end; ++x)
    {
        for (auto y=ins.cuboid.y.start; y<=ins.cuboid.y.end; ++y)
        {
            for (auto z=ins.cuboid.z.start; z<=ins.cuboid.z.end; ++z)
            {
                grid[x][y][z] = t;
            }
        }
    }
}

// Limit and transform instructions so they fit the grid size
std::vector<Instruction> trafo_instructions(const std::vector<Instruction> &ins_vec, const pCoord lower_lim, const pCoord upper_lim)
{
    std::vector<Instruction> limited_ins_vec;
    for (const auto &ins : ins_vec)
    {
        Instruction new_ins{ ins };
        new_ins.cuboid.x.start += SIZE;
        new_ins.cuboid.x.end += SIZE;
        new_ins.cuboid.y.start += SIZE;
        new_ins.cuboid.y.end += SIZE;
        new_ins.cuboid.z.start += SIZE;
        new_ins.cuboid.z.end += SIZE;
        if (is_in_range(new_ins, lower_lim, upper_lim))
        {
            limit_point(new_ins, lower_lim, upper_lim);
            limited_ins_vec.push_back(new_ins);
        }
    }
    return limited_ins_vec;
}

void limit_point(Instruction &ins, const pCoord lower_lim, const pCoord upper_lim)
{
    if (ins.cuboid.x.start < lower_lim) ins.cuboid.x.start = lower_lim;
    if (ins.cuboid.y.start < lower_lim) ins.cuboid.y.start = lower_lim;
    if (ins.cuboid.z.start < lower_lim) ins.cuboid.z.start = lower_lim;
    if (ins.cuboid.x.end > upper_lim) ins.cuboid.x.end = upper_lim;
    if (ins.cuboid.y.end > upper_lim) ins.cuboid.y.end = upper_lim;
    if (ins.cuboid.z.end > upper_lim) ins.cuboid.z.end = upper_lim;
}

bool is_in_range(Instruction &ins, const pCoord lower_lim, const pCoord upper_lim)
{
    if (ins.cuboid.x.end < lower_lim) return false;
    if (ins.cuboid.y.end < lower_lim) return false;
    if (ins.cuboid.z.end < lower_lim) return false;
    if (ins.cuboid.x.start > upper_lim) return false;
    if (ins.cuboid.y.start > upper_lim) return false;
    if (ins.cuboid.z.start > upper_lim) return false;
    return true;
}

uint64_t count_active_cubes(const Grid &grid)
{
    uint64_t sum{ 0u };
    for (size_t x=0; x<grid.size(); ++x)
    {
        for (size_t y=0; y<grid.at(0).size(); ++y)
        {
            for (size_t z=0; z<grid.at(0).at(0).size(); ++z)
            {
                if (grid[x][y][z])
                {
                    ++sum;
                }
            }
        }
    }
    return sum;
}

void print_instructions(std::ostream &out, const std::vector<Instruction> &ins_vec)
{
    for (const auto &ins : ins_vec)
    {
        if (ins.type == InsType::ON) 
        {
            out << "on x=";
        }
        else 
        {
            out << "off x=";
        }
        out << ins.cuboid.x.start << ".." << ins.cuboid.x.end << ",y=" << ins.cuboid.y.start << ".." << ins.cuboid.y.end << ",z=" << ins.cuboid.z.start << ".." << ins.cuboid.z.end << "\n";
    }
}

std::vector<Instruction> get_instructions(const std::string& file_path)
{
    std::vector<Instruction> ins_vec{};
    std::fstream input_file;
    input_file.open(file_path,std::ios::in);
    if (input_file.is_open()){
        std::string input_line;
        while(getline(input_file, input_line)){  //read data from file object and put it into string.
            std::vector<pCoord> num_vec = parse_string_to_number_vec<pCoord>(input_line);
            std::vector<std::string> sub_strings = split_string(input_line, " ");
            InsType type{ InsType::OFF };
            if (sub_strings[0] == "on")
            {
                type = InsType::ON;
            }
            ins_vec.push_back({ type, {{num_vec[0], num_vec[1]}, {num_vec[2],num_vec[3]}, {num_vec[4], num_vec[5]} }});
        }
        input_file.close();   //close the file object.
    }
    return ins_vec;
}