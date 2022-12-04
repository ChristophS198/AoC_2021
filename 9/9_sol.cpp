#include <string>
#include <vector>
#include <set>
#include <numeric>
#include <algorithm>

#include "../utility.h"


struct HeightMap
{
    HeightMap(std::vector<std::vector<int>> map) : height_map{map}, map_rows{map.size()}, map_cols{map[0].size()} {};
    unsigned int calc_risk_level_sum();
    long long get_largest_basin_product(unsigned int num_largest_basins);

private: 
    unsigned int get_basin_size(const Point<size_t>& low_point);
    std::vector<Point<size_t>> get_low_points() const;
    unsigned int calc_basin_rec(std::set<Point<size_t>>& basin_points, const Point<size_t>& call_pt, unsigned int& basin_count) const;
    std::vector<std::vector<int>> height_map;
    size_t map_rows{};
    size_t map_cols{};
};

long long HeightMap::get_largest_basin_product(unsigned int num_largest_basins)
{
    std::vector<Point<size_t>> low_points = get_low_points();
    std::vector<long long> basin_vec{};
    for (const auto& point : low_points)
    {
        basin_vec.push_back(static_cast<long long>(get_basin_size(point)));
    }
    std::sort(basin_vec.begin(), basin_vec.end());
    if (num_largest_basins < basin_vec.size())
    {
        return std::accumulate(basin_vec.end()-num_largest_basins, basin_vec.end(), 1, std::multiplies<>());
    }
    else
    {
        return std::accumulate(basin_vec.begin(), basin_vec.end(), 1, std::multiplies<>());
    }
}

long long get_largest_basin_product(const std::string& height_map_file)
{
    HeightMap height_map(read_2d_vec_from_file<int>(height_map_file));
    return height_map.get_largest_basin_product(3);
}

unsigned int get_risk_level_sum(const std::string& height_map_file)
{
    HeightMap height_map(read_2d_vec_from_file<int>(height_map_file));
    return height_map.calc_risk_level_sum();
}

unsigned int HeightMap::calc_basin_rec(std::set<Point<size_t>>& basin_points, const Point<size_t>& call_pt, unsigned int& basin_count) const
{
    const unsigned int cur_level = height_map[call_pt.x][call_pt.y];

    // check point one row above
    if (call_pt.x != 0)
    {
        unsigned int level_above = height_map[call_pt.x-1][call_pt.y];
        if (level_above > cur_level && level_above != 9 && 
            basin_points.count(Point<size_t>(call_pt.x-1, call_pt.y)) == 0)
        {
            ++basin_count;
            basin_points.emplace(call_pt.x-1,call_pt.y);
            calc_basin_rec(basin_points, Point<size_t>{call_pt.x-1, call_pt.y}, basin_count);
        }
    }

    // check point one row below
    if (call_pt.x != map_rows-1)
    {
        unsigned int level_below = height_map[call_pt.x+1][call_pt.y];
        if (level_below > cur_level && level_below != 9 && 
            basin_points.count(Point<size_t>(call_pt.x+1, call_pt.y)) == 0)
        {
            ++basin_count;
            basin_points.emplace(call_pt.x+1,call_pt.y);
            calc_basin_rec(basin_points, Point<size_t>{call_pt.x+1, call_pt.y}, basin_count);
        }
    }

    // check point one col to the left
    if (call_pt.y != 0)
    {
        unsigned int level_left = height_map[call_pt.x][call_pt.y-1];
        if (level_left > cur_level && level_left != 9 && 
            basin_points.count(Point<size_t>(call_pt.x, call_pt.y-1)) == 0)
        {
            ++basin_count;
            basin_points.emplace(call_pt.x, call_pt.y-1);
            calc_basin_rec(basin_points, Point<size_t>{call_pt.x, call_pt.y-1}, basin_count);
        }
    }

    // check point one col to the right
    if (call_pt.y != map_cols-1)
    {
        unsigned int level_right = height_map[call_pt.x][call_pt.y+1];
        if (level_right > cur_level && level_right != 9 && 
            basin_points.count(Point<size_t>(call_pt.x, call_pt.y+1)) == 0)
        {
            ++basin_count;
            basin_points.emplace(call_pt.x, call_pt.y+1);
            calc_basin_rec(basin_points, Point<size_t>{call_pt.x, call_pt.y+1}, basin_count);
        }
    }
    return basin_count;
}


unsigned int HeightMap::get_basin_size(const Point<size_t>& low_point)
{
    std::set<Point<size_t>> basin_points{low_point};
    unsigned int basin_size{1}; ///< start with 1, because local minimum represents a basin size of 1
    calc_basin_rec(basin_points, low_point, basin_size);
    return basin_size;
}


unsigned int HeightMap::calc_risk_level_sum()
{
    unsigned int risk_level{0}; 
    std::vector<Point<size_t>> low_points = get_low_points();
    for (const auto& point : low_points)
    {
        risk_level += height_map[point.x][point.y] + 1;
    }
    return risk_level;
}

std::vector<Point<size_t>> HeightMap::get_low_points() const
{
    std::vector<Point<size_t>> low_points{}; ///< not needed for task 1
    for (size_t row=0; row < height_map.size(); ++row)
    {
        bool above = row == 0 ? false : true;
        bool below = row == height_map.size()-1 ? false : true;
        for (size_t col=0; col < height_map[0].size(); ++col)
        {
            bool is_low_point = true;
            bool left  = col == 0 ? false : true;
            bool right = col == height_map[0].size()-1 ? false : true;
            if (above && height_map[row][col] >= height_map[row-1][col])
            {
                is_low_point = false; 
                continue;
            }
            if (below && height_map[row][col] >= height_map[row+1][col])
            {
                is_low_point = false; 
                continue;
            }
            if (left && height_map[row][col] >= height_map[row][col-1])
            {
                is_low_point = false; 
                continue;
            }
            if (right && height_map[row][col] >= height_map[row][col+1])
            {
                is_low_point = false; 
                continue;
            }
            if (is_low_point) 
            {
                low_points.push_back(Point<size_t>(row, col));
            }
        }
    }
    return low_points;
}