#include <string>
#include <map>
#include <algorithm>

#include "../utility.h"

using RuleMap = std::map<std::string, std::string>; ///< Used to map two characters to a one character string
using ElementMap = std::map<std::string, uint64_t>; ///< Used to store how often a character or pair of characters occurs
using PairInsertionMap = std::map<std::string,ElementMap>; ///< Used to store how a pair develops after x steps (how often each generated pair appears after x step)

class Polymer
{
public:
    Polymer(const std::string& file_path);
    unsigned long long multi_step(size_t num_steps, bool debug_on=false);
    uint64_t multi_step_40();

private:
    void step_rec(ElementMap &element_map, char a, char b, const size_t max_steps, size_t step, const bool debug_on=false);
    void step_insertion(PairInsertionMap &pair_map, const std::string pair, const size_t num_steps);
    void count_pairs(PairInsertionMap &pair_map, std::string str, const size_t max_steps);
    std::string m_polymer;
    RuleMap m_rule_map;
    PairInsertionMap m_pair_map;
};

uint64_t Polymer::multi_step_40()
{
    PairInsertionMap insert_map;
    ElementMap pair_map; // This map counts how often unique paris occur before the next iteration of x*10 steps is executed
    ElementMap pair_counter_after; // This map counts how often unique pairs occur after an iteration of x*10 steps
    for (size_t i=0; i<m_polymer.size()-1; ++i) // count how often each pair occurs in the original string
    {
        pair_map[m_polymer.substr(i,2)]++;
    }
    for (size_t i=0; i<4; ++i)
    {
        for (const auto p1 : pair_map) // for each unique pair that occurs in the current string look up or calc in what pairs they result after 10 steps
        {
            if (!insert_map.count(p1.first)) // do we already know how this pair evolves after 10 steps?
            {
                step_insertion(insert_map, p1.first, 10);
            }
            for (const auto& p2 : insert_map[p1.first])
            {
                pair_counter_after[p2.first] += p1.second*p2.second;
            }
        }
        pair_map = pair_counter_after;
        pair_counter_after.clear();
    }

    // pair_map.clear();
    std::vector<uint64_t> count_vec(26,0);
    for (const auto& p : pair_map)
    {
        count_vec[p.first[0]-'A'] += p.second;
    }
    // the last element of our inital polymer is counted separately for it is not considered in above for loop
    ++count_vec[m_polymer.back()-'A']; 
    std::sort(count_vec.begin(), count_vec.end());
    for(auto& v : count_vec)
    {
        if(v!=0) // iterate through count_vec until first element with value > 0
        {
            return (count_vec[25]-v);
        }
    }
    throw std::runtime_error("Unable to calc diff between max and min elements!");
}


void Polymer::step_insertion(PairInsertionMap& pair_map, const std::string pair, const size_t num_steps)
{
    if (pair.length() != 2)
    {
        throw std::invalid_argument("Only for strings of len 2!");
    }

    // construct string after num_steps steps
    std::string polymer = pair;
    for (size_t i=0; i<num_steps; ++i) 
    {
        std::string tmp;
        for (size_t p=0; p<polymer.size()-1; ++p)
        {
            tmp.push_back(polymer[p]);
            if (m_rule_map.count(polymer.substr(p,2)))
            {
                tmp += m_rule_map[polymer.substr(p,2)];
            }
        }
        tmp.push_back(polymer.back());
        polymer = tmp;
    }

    // count resulting pairs after num_steps steps
    auto &entry = pair_map[pair];
    for (size_t p=0; p<polymer.size()-1; ++p)
    {
        ++entry[polymer.substr(p,2)];
    }
}

unsigned long long Polymer::multi_step(size_t num_steps, bool debug_on)
{
    ElementMap element_map{};
    for (size_t i=0; i<m_polymer.size()-1; ++i)
    {
        if (debug_on)
        {
            std::cout << m_polymer[i];
        }
        ++element_map[std::string(1,m_polymer[i])];
        step_rec(element_map, m_polymer[i], m_polymer[i+1], num_steps, 0u, debug_on);
    }
    ++element_map[std::string(1,m_polymer[m_polymer.size()-1])];
    if (debug_on)
    {
        std::cout << m_polymer[m_polymer.size()-1] << std::endl;
    }
    size_t len{0};
    size_t min_val{element_map.begin()->second};
    size_t max_val{min_val};
    for (const auto& elem : element_map) 
    {
        len += elem.second;
        min_val = elem.second < min_val ? elem.second : min_val;
        max_val = elem.second > max_val ? elem.second : max_val;
        if (debug_on) 
        {
            std::cout << elem.first << ": " << elem.second << std::endl;
        }
    }
    std::cout << "Size: " << len << std::endl;
    return max_val - min_val;
}

void Polymer::step_rec(ElementMap &element_map, char a, char b, const size_t max_steps, size_t step, const bool debug_on)
{
    if (step < max_steps)
    {
        auto it=m_rule_map.find(std::string(1,a)+b);
        if ( it != m_rule_map.end())
        {
            ++element_map[it->second];
            ++step;
            step_rec(element_map, a, it->second[0], max_steps, step, debug_on);
            if (debug_on)
            {
                std::cout << it->second;
            }
            step_rec(element_map, it->second[0], b, max_steps, step, debug_on);
        }
    }
    // Expected for NN: NBBNBNBBCCNBCNCCN
}



Polymer::Polymer(const std::string& file_path)
{
    std::vector<std::string> file_input = read_string_vec_from_file(file_path);
    m_polymer = file_input[0];
    for (size_t i=2; i<file_input.size(); ++i)
    {
        std::string line = file_input[i];
        if (!line.empty())
        {
            std::vector<std::string> str_vec = parse_string_to_substring(line, " -> ");
            if (str_vec.size() != 2)
            {
                std::string err_str = std::string("Error on input line: ") + std::to_string(i);
                throw std::runtime_error(err_str);
            }
            m_rule_map[str_vec[0]] = str_vec[1];
        }
    }
}