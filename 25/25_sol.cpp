#include <string>
#include <vector>
#include <ostream>

#include "../utility.h"

using Elem = char;

constexpr char EAST{ '>' };
constexpr char SOUTH{ 'v' };
constexpr char EMPTY{ '.' };
constexpr char BLOCK{ 'x' }; // used to mark positions that do not need to be checked

enum class Dir{
    East,
    South
};

/**
 * @brief extend sea_floor by 1 row and 1 column by mirroring the first row and first column
 * This allows an in place operation for the cucumbers movement
 * 
 * @param sea_floor 
 */
void extend_sea_floor(std::vector<std::vector<Elem>> &sea_floor);

int move_east_facing_cucumbers(std::vector<std::vector<Elem>> &sea_floor);
int move_south_facing_cucumbers(std::vector<std::vector<Elem>> &sea_floor);
void print_sea_floor(std::ostream &out, const std::vector<std::vector<Elem>> &sea_floor);


std::uint64_t day_25_1(const std::string &file_path)
{
    std::vector<std::vector<Elem>> sea_floor = read_2d_vec_from_file<char>(file_path);
    // extend_sea_floor(sea_floor);
    std::uint64_t num_steps{ 0u };

    int num_moves{ 1 };
    while (num_moves > 0)
    {
        num_moves = move_east_facing_cucumbers(sea_floor);
        num_moves += move_south_facing_cucumbers(sea_floor);
        ++num_steps;
    }

    return num_steps;
}

void extend_sea_floor(std::vector<std::vector<Elem>> &sea_floor)
{
    sea_floor.push_back(sea_floor[0]);
    for (auto &row : sea_floor)
    {
        row.push_back(row[0]);
    }
}

int move_east_facing_cucumbers(std::vector<std::vector<Elem>> &sea_floor)
{
    auto row_size{ sea_floor.size() };
    auto col_size{ sea_floor[0].size() };
    int num_moves{ 0 };
    std::vector<Elem> old_first_col;
    old_first_col.reserve(col_size);

    for (auto row=0u; row<row_size; ++row)
    {
        old_first_col.push_back(sea_floor[row][0]);
        for (auto col=0u; col<col_size-1; ++col)
        {
            auto &cur = sea_floor[row][col];
            auto &neighbor = sea_floor[row][col+1];
            // check if neighboring place is empty
            if (EAST == cur && EMPTY == neighbor)
            {
                if (col == col_size-2)
                {
                    old_first_col[row] = BLOCK; // mark this row so we do not move this cucumber when handling the last row
                }
                cur = EMPTY;
                neighbor = EAST;
                ++num_moves;
                ++col; // increase col so we do no move this fellow again
            }
        }
    }

    // special treatment for the last col
    for (auto row=0u; row<row_size; ++row)
    {
        auto &cur = sea_floor[row][col_size-1];
        // check in old first row, if the neighbor is empty
        if (EAST == cur && EMPTY == old_first_col[row])
        {
            cur = EMPTY;
            sea_floor[row][0] = EAST;
            ++num_moves;
        }
    }
    return num_moves;
}
int move_south_facing_cucumbers(std::vector<std::vector<Elem>> &sea_floor)
{
    auto row_size{ sea_floor.size() };
    auto col_size{ sea_floor[0].size() };
    int num_moves{ 0 };
    std::vector<Elem> old_first_row = sea_floor[0];

    for (auto col=0u; col<col_size; ++col)
    {
        for (auto row=0u; row<row_size-1; ++row)
        {
            auto &cur = sea_floor[row][col];
            auto &neighbor = sea_floor[row+1][col];
            // check if neighboring place is empty
            if (SOUTH == cur && EMPTY == neighbor)
            {
                if (row == row_size-2)
                {
                    old_first_row[col] = BLOCK; // mark this col so we do not move this cucumber when handling the last row
                }
                cur = EMPTY;
                neighbor = SOUTH;
                ++num_moves;
                ++row; // increase row so we do no move this fellow again
            }
        }
    }

    // special treatment for the last row
    for (auto col=0u; col<col_size; ++col)
    {
        auto &cur = sea_floor[row_size-1][col];
        // check in old first row, if the neighbor is empty
        if (SOUTH == cur && EMPTY == old_first_row[col])
        {
            cur = EMPTY;
            sea_floor[0][col] = SOUTH;
            ++num_moves;
        }
    }
    
    return num_moves;
}

void print_sea_floor(std::ostream &out, const std::vector<std::vector<Elem>> &sea_floor)
{
    auto row_size{ sea_floor.size() };
    auto col_size{ sea_floor[0].size() };

    for (auto row=0u; row<row_size; ++row)
    {
        for (auto col=0u; col<col_size; ++col)
        {
            out << sea_floor[row][col];
        }
        out << "\n";
    }
    out << "\n";
}