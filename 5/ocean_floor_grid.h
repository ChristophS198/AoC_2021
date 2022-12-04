#include <iostream>
#include <string>
#include <vector>

#include "../utility.h"

template<typename T>
struct Line {
    Point<T> start;
    Point<T> end;
};

class OceanFloorGrid 
{
public:

    /**
     * @brief Construct a new Ocean Floor Grid object with initial size
     * 
     * @param dim initial size of 2d quadratic grid
     */
    OceanFloorGrid(size_t dim=1000) : m_dim{dim}, m_grid(dim,std::vector<unsigned int>(dim)) {};

    /**
     * @brief Get the number of points with an overlap count larger x (threshold)
     * 
     * @param threshold number of overlaps for a grid point must exceed threshold in order to be counted
     * @return unsigned int number of points in the grid where at least threshold+1 overlaps occur
     */
    unsigned int get_num_points_larger_x(const unsigned int threshold) const;
    
    /**
     * @brief add a horizontal or vertical hydrothermal vent line to the grid
     * 
     * @param line line containing start and end point of hydrothermal vent
     */
    void add_hor_vert_line(const Line<unsigned int>& line);

    void add_line(const Line<unsigned int>& line);

    /**
     * @brief prints grid content to a ostream for debug purposes
     * 
     * @param out 
     * @return std::ostream& 
     */
    std::ostream& print_grid(std::ostream& out) const;

private:
    /**
     * @brief Resizes grid while keeping old values, if during add operation of a line a value outside of current grid is detected
     * 
     * @param new_dim dimension of new grid
     */
    void resize_grid(const size_t new_dim);

    size_t m_dim{};
    std::vector<std::vector<unsigned int>> m_grid{}; ///< 2D Array for resemblance of ocean floor. Entries contain a
};


unsigned int OceanFloorGrid::get_num_points_larger_x(const unsigned int threshold) const
{
    unsigned int num{0};
    for (const auto& row : m_grid) 
    {
        for (const auto& elem : row)
        {
            if (elem > threshold)
            {
                ++num;
            }
        }
    }
    return num;
}


void OceanFloorGrid::add_hor_vert_line(const Line<unsigned int>& line)
{
    Point<unsigned int> line_start{line.start};
    Point<unsigned int> line_end{line.end};
    if (line.start.x != line.end.x && line.start.y != line.end.y) // Only horizontal and vertical lines are considered
    {
        return;
    }
    if (line_start.x > line_end.x || line_start.y > line_end.y)
    {
        line_start = line.end;
        line_end = line.start;
    }

    // Error checks
    if (line_start.x < 0 || line_start.y < 0)
    {
        throw std::runtime_error("Negative coordinate in x or y component during adding of horizontal or vertical line!");
    }    
    
    // if current grid size too small, increase dimension
    if(static_cast<size_t>(line_end.x) > m_dim || static_cast<size_t>(line_end.y) > m_dim)
    {
        size_t new_dim = line_end.x > line_end.y ? line_end.x : line_end.y;
        resize_grid(new_dim);
    }

    // increase each grid entry between line_start and line_end
    for (size_t row = line_start.x; row <= line_end.x; ++row)
    {
        for (size_t col = line_start.y; col <= line_end.y; ++col)
        {
            ++m_grid[row][col];
        }
    }
}

void OceanFloorGrid::resize_grid(const size_t new_dim)
{
    m_grid.resize(new_dim);
    for (auto& vec : m_grid) 
    {
        vec.resize(new_dim);
    }
    m_dim = new_dim;
}

std::ostream& OceanFloorGrid::print_grid(std::ostream& out) const
{
    for (const auto& row : m_grid)
    {
        for (const auto& val : row) 
        {
            out << val << " ";
        }
        out << "\n";
    }
    return out;
}




void OceanFloorGrid::add_line(const Line<unsigned int>& line)
{
    // if current grid size too small, increase dimension
    unsigned int max_val = std::max(std::max(std::max(line.start.x, line.start.y), line.end.x), line.end.y);
    if(static_cast<size_t>(max_val) > m_dim)
    {
        size_t new_dim = static_cast<size_t>(max_val);
        resize_grid(new_dim);
    }

    // Calculate direction components gradient vector of the input line  
    int x_diff = line.end.x - line.start.x;
    int x_off = x_diff < 0 ? -1 : (x_diff == 0 ? 0 : 1);
    int y_diff = line.end.y - line.start.y;
    int y_off = y_diff < 0 ? -1 : (y_diff == 0 ? 0 : 1);
    
    // Get number of steps from start to end point of line
    int num_steps = x_diff == 0 ? std::abs(y_diff) : std::abs(x_diff);
    num_steps += 1; // Add one step for both endpoints are included in line
    
    size_t row = static_cast<size_t>(line.start.x);
    size_t col = static_cast<size_t>(line.start.y);
    for (int i=0; i<num_steps; ++i)
    {
        ++m_grid[row][col];
        row += x_off;
        col += y_off;
    }
}