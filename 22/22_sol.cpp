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

struct Instruction
{
    InsType type;
    Point3D<pCoord> start;
    Point3D<pCoord> end;
};

void print_instructions(std::ostream &out, const std::vector<Instruction> &ins_vec);
std::vector<Instruction> get_instructions(const std::string& file_path);
std::vector<Instruction> trafo_instructions(const std::vector<Instruction> &ins_vec, const pCoord lower_lim, const pCoord upper_lim);
void limit_point(Instruction &ins, const pCoord lower_lim, const pCoord upper_lim);
bool is_in_range(Instruction &ins, const pCoord lower_lim, const pCoord upper_lim);
void execute_instruction(const Instruction &ins, Grid &grid);
uint64_t count_active_cubes(const Grid &grid);

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


std::uint64_t day_22_2(const std::string &file_path)
{

    return 0u;
}

void execute_instruction(const Instruction &ins, Grid &grid)
{
    uint8_t t{ 0u };
    if (InsType::ON == ins.type)
    {
        t = 1u;
    }
    for (auto x=ins.start.x; x<=ins.end.x; ++x)
    {
        for (auto y=ins.start.y; y<=ins.end.y; ++y)
        {
            for (auto z=ins.start.z; z<=ins.end.z; ++z)
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
        new_ins.start = new_ins.start + SIZE;
        new_ins.end = new_ins.end + SIZE;
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
    if (ins.start.x < lower_lim) ins.start.x = lower_lim;
    if (ins.start.y < lower_lim) ins.start.y = lower_lim;
    if (ins.start.z < lower_lim) ins.start.z = lower_lim;
    if (ins.start.x > upper_lim) ins.end.x = upper_lim;
    if (ins.start.y > upper_lim) ins.end.y = upper_lim;
    if (ins.start.z > upper_lim) ins.end.z = upper_lim;
}

bool is_in_range(Instruction &ins, const pCoord lower_lim, const pCoord upper_lim)
{
    if (ins.end.x < lower_lim) return false;
    if (ins.end.y < lower_lim) return false;
    if (ins.end.z < lower_lim) return false;
    if (ins.start.x > upper_lim) return false;
    if (ins.start.y > upper_lim) return false;
    if (ins.start.z > upper_lim) return false;
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
        out << ins.start.x << ".." << ins.end.x << ",y=" << ins.start.y << ".." << ins.end.y << ",z=" << ins.start.z << ".." << ins.end.z << "\n";
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
            ins_vec.push_back({ type, {num_vec[0], num_vec[2], num_vec[4]},{num_vec[1], num_vec[3], num_vec[5]} });
        }
        input_file.close();   //close the file object.
    }
    return ins_vec;
}