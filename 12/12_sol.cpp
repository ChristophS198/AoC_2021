#include <vector>
#include <map>
#include <string>
#include <memory>
#include <algorithm>
#include <iostream>

#include "../utility.h"

using t_cavern_id = std::string;

struct Cavern
{
    t_cavern_id m_id;
    bool m_is_large;
    Cavern(t_cavern_id id, bool is_large) : m_id{id}, m_is_large{is_large} {};
    bool operator==(const Cavern& cave) const {return this->m_id == cave.m_id;};
};
using t_cavern_map = std::map<t_cavern_id, std::vector<std::shared_ptr<Cavern>>>;

// function declarations
t_cavern_map read_cavern_map(const std::string& file_path, const std::string& start="start", const std::string& end="end");
void print_cavern_map(const t_cavern_map& cavern_map);
unsigned int calc_paths(const t_cavern_map& cavern_map, const std::string& start="start", const std::string& end="end");
unsigned int calc_path_rec(const t_cavern_map& cavern_map, std::vector<t_cavern_id> path, std::shared_ptr<Cavern> cur_cavern, unsigned int& num_paths, const t_cavern_id& start, const t_cavern_id& end, bool small_cave_joker);


unsigned int get_num_paths(const std::string& file_path)
{ 
    t_cavern_map cavern_map = read_cavern_map(file_path);
    // print_cavern_map(cavern_map);
    return calc_paths(cavern_map);
}

t_cavern_map read_cavern_map(const std::string& file_path, const std::string& start, const std::string& end)
{
    std::vector<std::string> cavern_vec = read_string_vec_from_file(file_path);
    t_cavern_map cavern_map;
    for (const auto& connection : cavern_vec)
    {
        std::vector<std::string> sub_str = parse_string_to_substring(connection, "-");
        if (sub_str.size() == 0) // might occur on the last (empty) input line
        {
            continue;
        }
        else 
        {
            if (sub_str.size() != 2)
            {
                throw std::runtime_error("A connection must consist of a pair of strings!");
            }
            else
            {
                auto vec = cavern_map[sub_str[0]]; // should also construct new entry if it does not already exist
                auto is_found_0 = [sub_str](std::shared_ptr<Cavern> cave){ return sub_str[0] == cave->m_id; };
                auto is_found_1 = [sub_str](std::shared_ptr<Cavern> cave){ return sub_str[1] == cave->m_id; };
                if (std::find_if(vec.begin(), vec.end(), is_found_1) == vec.end()) // connection does not exist
                {
                    bool is_large_cave = std::isupper(static_cast<unsigned char>(sub_str[1][0]));
                    cavern_map[sub_str[0]].push_back(std::make_shared<Cavern>(sub_str[1], is_large_cave));
                }
                vec = cavern_map[sub_str[1]]; // should also construct new entry if it does not already exist
                if (std::find_if(vec.begin(), vec.end(), is_found_0) == vec.end()) // connection does not exist
                {
                    bool is_large_cave = std::isupper(static_cast<unsigned char>(sub_str[0][0]));
                    cavern_map[sub_str[1]].push_back(std::make_shared<Cavern>(sub_str[0], is_large_cave));
                }
            }
        }
    }
    return cavern_map;
}


void print_cavern_map(const t_cavern_map& cavern_map)
{
    std::cout << "\n";
    for (const auto& elem : cavern_map)
    {
        for (const auto& connection : elem.second)
        {
            std::cout << elem.first << " - " << connection->m_id << "\n";
        }
    }
    std::cout << "\n";
}

/**
 * @brief Find all paths by starting at the endpoint and going backwards all possible
 * connections that lead to the startpoint
 * 
 * @param cavern_map map storing all connections between caverns and a flag for small/large cavern
 * @param start starting point 
 * @param end ending point
 * @return unsigned int number of unique paths from start to end
 */
unsigned int calc_paths(const t_cavern_map& cavern_map, const t_cavern_id& start, const t_cavern_id& end)
{
    std::vector<t_cavern_id> path{end};
    unsigned int num_paths{0};
    for (const auto& cavern : cavern_map.at(end)) 
    {
        calc_path_rec(cavern_map, path, cavern, num_paths, start, end, true);
    }
    return num_paths;
}


unsigned int calc_path_rec(const t_cavern_map& cavern_map, std::vector<t_cavern_id> path, std::shared_ptr<Cavern> cur_cavern, unsigned int& num_paths, const t_cavern_id& start, const t_cavern_id& end, bool small_cave_joker)
{
    if (cur_cavern->m_id == start) // path from start to end found
    {
        path.push_back(cur_cavern->m_id);
        return ++num_paths;
    }
    if (cur_cavern->m_id == end) 
    {
        return num_paths;
    }
    if (std::find(path.begin(), path.end(), cur_cavern->m_id) != path.end()) // cavern already visited in current travel
    {
        if (!cur_cavern->m_is_large) // small cavern that has already been visited 
        {
            if (small_cave_joker) // only one small cavern can be visited twice per route
            {
                path.push_back(cur_cavern->m_id);
                for (const auto& cavern : cavern_map.at(cur_cavern->m_id)) 
                {
                    calc_path_rec(cavern_map, path, cavern, num_paths, start, end, false);
                }
            }
            return num_paths;
        }
        else
        {
            path.push_back(cur_cavern->m_id);
            for (const auto& cavern : cavern_map.at(cur_cavern->m_id)) 
            {
                calc_path_rec(cavern_map, path, cavern, num_paths, start, end, small_cave_joker);
            }
            return num_paths;
        }
    }
    else // new cavern 
    {
        path.push_back(cur_cavern->m_id);
        for (const auto& cavern : cavern_map.at(cur_cavern->m_id)) 
        {
            calc_path_rec(cavern_map, path, cavern, num_paths, start, end, small_cave_joker);
        }
        return num_paths;
    }
}
