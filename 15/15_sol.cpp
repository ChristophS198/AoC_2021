#include <string>
#include <array>
#include <iostream>
#include <limits>
#include <algorithm>
#include <memory>

#include "../utility.h"
#include "sorted_queue.h"

constexpr size_t FIELD_SIZE = 100;
constexpr size_t EXT_FACTOR = 5;
constexpr size_t FIELD_SIZE_2 = FIELD_SIZE * EXT_FACTOR;

template<typename T, typename U>
struct DijkstraPt : Point<T>
{
    DijkstraPt() : Point<T>(), dist{}, is_visited{false}, is_reachable{false}, predecessor{} {};
    DijkstraPt(const T& x1, const T& y1, const U& w, bool v, bool r, Point<T> p) 
        : Point<T>(x1,y1), dist{w}, is_visited{v}, is_reachable{r}, predecessor{p} {};
    bool operator<(const DijkstraPt &other) const;
    bool operator==(const DijkstraPt &other) const;
    bool operator==(const Point<T> &other) const;

    U dist;
    bool is_visited{false};
    bool is_reachable{false};
    Point<T> predecessor{};
};

template<typename T, typename U>
bool DijkstraPt<T,U>::operator<(const DijkstraPt &other) const
{
    return this->dist < other.dist;
}

template<typename T, typename U>
bool DijkstraPt<T,U>::operator==(const DijkstraPt &other) const
{
    return this->x == other.x && this->y == other.y;
}
template<typename T, typename U>
bool DijkstraPt<T,U>::operator==(const Point<T> &other) const
{
    return this->x == other.x && this->y == other.y;
}


using tCoord = std::uint32_t;
using tWeight = std::uint32_t;
using tPoint = DijkstraPt<tCoord, tWeight>;
template<size_t ArraySize>
using t2dShortestPathArr = std::array<std::array<tPoint, ArraySize>, ArraySize>;
template<size_t ArraySize>
using t2dWeightArr = std::array<std::array<tCoord, ArraySize>, ArraySize>;
using tReachablePts = std::vector<tPoint*>;

// Function declarations
template<size_t ArraySize>
void update_not_visited_neighbors(tReachablePts &reachable_pts, t2dShortestPathArr<ArraySize> &shortest_path, tPoint next_pt, const t2dShortestPathArr<ArraySize> &weights);
template<size_t ArraySize>
bool update_DijkstraPt(tPoint pt, t2dShortestPathArr<ArraySize> & shortest_paths, tReachablePts &reachable_pts);
template<size_t ArraySize>
const t2dShortestPathArr<ArraySize> get_shortest_path(Point<tCoord> start, Point<tCoord> end, t2dWeightArr<ArraySize> weights);
void sort(tReachablePts &tReachablePts);
template<size_t ArraySize>
void vertical_ext(t2dWeightArr<ArraySize> &weights);
template<size_t ArraySize>
void horizontal_ext(t2dWeightArr<ArraySize> &weights);

std::uint32_t calc_lowest_risk(const std::string& file_path)
{
    std::vector<std::vector<tCoord>> chiton_vec = read_2d_vec_from_file<tCoord>(file_path);
    t2dWeightArr<FIELD_SIZE> chiton_arr;
    for (size_t row=0; row < chiton_vec.size(); ++row)
    {
        std::copy_n(chiton_vec[row].begin(), chiton_vec[0].size(), chiton_arr[row].begin());
    }
    Point<tCoord> start{0,0};
    Point<tCoord> end{FIELD_SIZE-1, FIELD_SIZE-1};
    t2dShortestPathArr<FIELD_SIZE> result = get_shortest_path<FIELD_SIZE>(start, end, chiton_arr);
    return result[end.x][end.y].dist;
}


std::uint32_t calc_lowest_risk_2(const std::string& file_path)
{
    std::vector<std::vector<tCoord>> chiton_vec = read_2d_vec_from_file<tCoord>(file_path);
    t2dWeightArr<FIELD_SIZE_2> chiton_arr;
    for (size_t row=0; row < chiton_vec.size(); ++row)
    {
        std::copy_n(chiton_vec[row].begin(), chiton_vec[0].size(), chiton_arr[row].begin());
    }
    vertical_ext<FIELD_SIZE_2>(chiton_arr);
    horizontal_ext<FIELD_SIZE_2>(chiton_arr);
    Point<tCoord> start{0,0};
    Point<tCoord> end{chiton_arr.size()-1, chiton_arr.size()-1};
    t2dShortestPathArr<FIELD_SIZE_2> result = get_shortest_path<FIELD_SIZE_2>(start, end, chiton_arr);
    return result[end.x][end.y].dist;
}

template<size_t ArraySize>
void vertical_ext(t2dWeightArr<ArraySize> &weights)
{
    for (size_t row=FIELD_SIZE; row < weights.size(); ++row)
    {
        for (size_t col=0; col < FIELD_SIZE; ++col)
        {
            weights[row][col] = weights[row-FIELD_SIZE][col] + 1;
            if (weights[row][col] == 10)
            {
                weights[row][col] = 1;
            }
        }
    }
}

template<size_t ArraySize>
void horizontal_ext(t2dWeightArr<ArraySize> &weights)
{
    for (size_t row=0; row < weights.size(); ++row)
    {
        for (size_t col=FIELD_SIZE; col < weights[0].size(); ++col)
        {
            weights[row][col] = weights[row][col-FIELD_SIZE] + 1;
            if (weights[row][col] == 10)
            {
                weights[row][col] = 1;
            }
        }
    }
}


template<size_t ArraySize>
const t2dShortestPathArr<ArraySize> get_shortest_path(Point<tCoord> start, Point<tCoord> end, t2dWeightArr<ArraySize> weights)
{
    t2dShortestPathArr<ArraySize> shortest_path{};
    shortest_path[start.x][start.y] = tPoint(start.x, start.y, 0, true, true, Point<tCoord>{0,0});
    tReachablePts reachable_pts{&shortest_path[start.x][start.y]}; // Init queue of reachable points with start point
    while (!reachable_pts.empty())
    {
        auto next_pt = reachable_pts.back(); // get point with min dist to any of the already visited points
        reachable_pts.pop_back();
        next_pt->is_visited = true;
        if (!(*next_pt == start))
        {
            next_pt->dist = weights[next_pt->x][next_pt->y] + shortest_path[next_pt->predecessor.x][next_pt->predecessor.y].dist;
        }

        if (*next_pt == end) 
        {
            break;
        }
        update_not_visited_neighbors<ArraySize>(reachable_pts, shortest_path, *next_pt, weights);
    }
    return shortest_path;
}


/**
 * @brief Adds not visited neighbors of current node/point to the queue of reachable points. 
 * 
 * @tparam ArraySize 
 * @param reachable_pts vector of pointers to reachable points that have not been visited yet
 * @param shortest_path 
 * @param cur_pt 
 * @param weights 
 */
template<size_t ArraySize>
void update_not_visited_neighbors(tReachablePts &reachable_pts, t2dShortestPathArr<ArraySize> &shortest_path, tPoint cur_pt, const t2dWeightArr<ArraySize> &weights)
{
    tCoord cur_dist = shortest_path[cur_pt.x][cur_pt.y].dist; // Distance to cur_pt
    bool above_exist = cur_pt.x > 0;
    bool below_exist = cur_pt.x < ArraySize-1;
    bool right_exist = cur_pt.y < ArraySize-1;
    bool left_exist  = cur_pt.y > 0;

    bool sorted{true};
    if (left_exist && !shortest_path[cur_pt.x][cur_pt.y-1].is_visited)
    {
        // tPoint neighbor{cur_pt.x-1, cur_pt.x, weights[cur_pt.x-1][cur_pt.x] + cur_dist, cur_pt};
        tPoint neighbor{cur_pt.x, cur_pt.y-1, weights[cur_pt.x][cur_pt.y-1] + cur_dist, false, true, cur_pt};
        sorted = update_DijkstraPt(neighbor, shortest_path, reachable_pts) && sorted;
    }
    if (right_exist && !shortest_path[cur_pt.x][cur_pt.y+1].is_visited)
    {
        tPoint neighbor{cur_pt.x, cur_pt.y+1, weights[cur_pt.x][cur_pt.y+1] + cur_dist, false, true, cur_pt};
        sorted = update_DijkstraPt(neighbor, shortest_path, reachable_pts) && sorted;
    }
    if (above_exist && !shortest_path[cur_pt.x-1][cur_pt.y].is_visited)
    {
        tPoint neighbor{cur_pt.x-1, cur_pt.y, weights[cur_pt.x-1][cur_pt.y] + cur_dist, false, true, cur_pt};
        sorted = update_DijkstraPt(neighbor, shortest_path, reachable_pts) && sorted;
    }
    if (below_exist && !shortest_path[cur_pt.x+1][cur_pt.y].is_visited)
    {
        tPoint neighbor{cur_pt.x+1, cur_pt.y, weights[cur_pt.x+1][cur_pt.y] + cur_dist, false, true, cur_pt};
        sorted = update_DijkstraPt(neighbor, shortest_path, reachable_pts) && sorted;
    }
    if (!sorted)
    {
        sort(reachable_pts);
    }
}

/**
 * @brief Update or add point with coord [x,y] to the priority queue if the point has not been visited yet
 * 
 * @tparam ArraySize 
 * @param pt can be seen as a path to reach coordinate [pt.x, pt.y]
 * @param shortest_paths 2D-array of points
 * @param reachable_pts queue of reachable points
 * @return true if the queue of reachable points was not modified and the queue is still sorted
 * @return false if the queue was modified, either by adding a new point or changing a existing one
 */
template<size_t ArraySize>
bool update_DijkstraPt(tPoint pt, t2dShortestPathArr<ArraySize> &shortest_paths, tReachablePts &reachable_pts)
{
    tPoint& cur_pt = shortest_paths[pt.x][pt.y];
    if (cur_pt.is_reachable)
    {
        if (pt.dist < cur_pt.dist)
        {
            cur_pt.dist = pt.dist;
            cur_pt.predecessor.x = pt.predecessor.x;
            cur_pt.predecessor.y = pt.predecessor.y;
            return false;
        }
    }
    else
    {
        cur_pt = pt;
        reachable_pts.push_back(&shortest_paths[pt.x][pt.y]);
        return false;
    }
    return true;
}


/**
 * @brief sorts the queue of reachable points so shortest paths are at the end of the vector
 * 
 * @param reachable_pts vector of reachable points (pointer to points)
 */
void sort(tReachablePts &reachable_pts)
{
    std::sort(reachable_pts.begin(), reachable_pts.end(), [](const tPoint *a, const tPoint *b) -> bool 
    {
        return (*b) < (*a);
    });
}


