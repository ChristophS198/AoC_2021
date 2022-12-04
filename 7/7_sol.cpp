#include <string>
#include <numeric>
#include <cmath>
#include <algorithm>
#include <map>

#include "../utility.h"


template <typename T>
inline const typename T::key_type& last_key(const T& pMap)
{
    return pMap.rbegin()->first;
}

unsigned int get_fuel(const std::map<unsigned int, unsigned int>& pos_count_map)
{
    unsigned int acc_dist{0}; ///< Total accumulated dist of all left/right-handed crabs to threshold
    unsigned int acc_fish_num{0}; ///< Sums up all crabs left/right of threshold
    unsigned int max_pos = last_key(pos_count_map); ///< Get largest position from map
    std::vector<unsigned int> dist_vec(max_pos+1); ///< This vector is used to store the L1 norm if center value is equal to index
    for (unsigned int pos=0; pos<=max_pos; ++pos)
    {
        acc_dist += acc_fish_num;
        if (pos_count_map.find(pos) != pos_count_map.end())
        {
            acc_fish_num += pos_count_map.at(pos); ///< accumulate number of fish that are left of threshold (pos)
        }
        dist_vec[pos] = acc_dist;
    }
    acc_dist = 0;
    acc_fish_num = 0;
    for (int pos=max_pos; pos>=0; --pos)
    {
        acc_dist += acc_fish_num;
        if (pos_count_map.find(pos) != pos_count_map.end())
        {
            acc_fish_num += pos_count_map.at(pos); ///< accumulate number of fish that are left of threshold (pos)
        }
        dist_vec[pos] += acc_dist;
    }

    return *(std::min_element(dist_vec.begin(), dist_vec.end()));
}

unsigned int get_fuel_2(const std::map<unsigned int, unsigned int>& pos_count_map)
{
    unsigned int acc_dist{0}; ///< Total accumulated dist of all left/right-handed crabs to threshold
    unsigned int acc_dist_incr{0}; ///< Increase of distance when the threshold is moved one step left or right
    unsigned int acc_fish_num{0}; ///< Sums up all crabs left/right of threshold
    unsigned int max_pos = last_key(pos_count_map); ///< Get largest position from map
    std::vector<unsigned int> dist_vec(max_pos+1); ///< This vector is used to store the L1 norm if center value is equal to index
    for (unsigned int pos=0; pos<=max_pos; ++pos)
    {
        acc_dist_incr += acc_fish_num;
        acc_dist += acc_dist_incr; 
        if (pos_count_map.find(pos) != pos_count_map.end())
        {
            acc_fish_num += pos_count_map.at(pos); ///< accumulate number of fish that are left of threshold (pos)
        }
        dist_vec[pos] = acc_dist;
    }
    acc_dist = 0;
    acc_dist_incr = 0;
    acc_fish_num = 0;
    for (int pos=max_pos; pos>=0; --pos)
    {
        acc_dist_incr += acc_fish_num;
        acc_dist += acc_dist_incr; 
        if (pos_count_map.find(pos) != pos_count_map.end())
        {
            acc_fish_num += pos_count_map.at(pos); ///< accumulate number of fish that are left of threshold (pos)
        }
        dist_vec[pos] += acc_dist;
    }

    return *(std::min_element(dist_vec.begin(), dist_vec.end()));
}

unsigned int calc_min_fuel(const std::string& file_path, bool second_star=true)
{
    std::vector<int> start_pos = read_numbers_from_file<int>(file_path);
    std::map<unsigned int, unsigned int> pos_count_map;
    for (const auto& pos : start_pos)
    {
        ++pos_count_map[pos];
    }
    if(second_star)
    {
        return get_fuel_2(pos_count_map);
    }
    else
    {
        return get_fuel(pos_count_map);
    }
}
