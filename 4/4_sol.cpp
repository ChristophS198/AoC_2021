#include <vector>
#include <iostream>
#include <sstream>

#include "bingo.h"


/**
 * @brief takes a string and splits it into multiple numbers of type T which are stored in the output vector
 * 
 * @param str input string containig multiple numbers of type T that are separated by delim (can be multiple delims concatenated)
 * @param delim delimiting character
 * @return std::vector<T> 
 */
template<typename T>
std::vector<T> split_string_in_num(const std::string& str, const char delim)
{
    std::vector<T> res;
    std::string nxt;

    // iterate through each char
    for (std::string::const_iterator it = str.begin(); it != str.end(); it++)
    {
        if (*it == delim)
        {
            if (!nxt.empty())
            {
                if( std::is_same<T, double>::value)                 res.push_back(std::stod(nxt));
                else if( std::is_same<T, float>::value)             res.push_back(std::stof(nxt));
                else if( std::is_same<T, unsigned int>::value)      res.push_back(static_cast<unsigned int>(std::stoul(nxt)));
                else if( std::is_same<T, unsigned long>::value)     res.push_back(std::stoul(nxt));
                else if( std::is_same<T, unsigned long long>::value)res.push_back(std::stoull(nxt));
                else if( std::is_same<T, int>::value)               res.push_back(std::stoi(nxt));
                else throw std::runtime_error("Unexpected type for split_string_in_num() received!");
                nxt.clear();
            }
        }
        else
        {
            nxt += *it;
        }
    }
    if (!nxt.empty())
    {
        if( std::is_same<T, double>::value)                 res.push_back(std::stod(nxt));
        else if( std::is_same<T, float>::value)             res.push_back(std::stof(nxt));
        else if( std::is_same<T, unsigned int>::value)      res.push_back(static_cast<unsigned int>(std::stoul(nxt)));
        else if( std::is_same<T, unsigned long>::value)     res.push_back(std::stoul(nxt));
        else if( std::is_same<T, unsigned long long>::value)res.push_back(std::stoull(nxt));
        else if( std::is_same<T, int>::value)               res.push_back(std::stoi(nxt));
        else throw std::runtime_error("Unexpected type for split_string_in_num() received!");
    }
    return res;
}

/**
 * @brief Reads numbers that are drawn and bingo init data from file and fills o_bingo_vec + returns numbers that are drawn
 * 
 * @tparam T type of entries in bingo game
 * @tparam BingoSize size of bingo table
 * @param file_path path where bingo init data is read from
 * @param o_bingo_vec vector of bingo tables that are filled based on data from input file
 * @return const std::vector<T> vector of numbers that correspond to the numbers that are drawn during the bingo game
 */
template<typename T, std::size_t BingoSize = 5>
const std::vector<T> read_bingo_input(const std::string& file_path, std::vector<Bingo<T, BingoSize>>& o_bingo_vec)
{
    std::vector<T> draw_vec{};
    std::fstream input_file;
    input_file.open(file_path,std::ios::in);
    if (input_file.is_open()){
        std::string input_line{};
        if(getline(input_file, input_line)){  //read data from file object and put it into string.
            std::istringstream first_line;
            first_line.str(input_line);
            std::string num{};  
            while(getline(first_line, num, ',')) // first line should be a comma ',' separated list of values
            {
                
                if( std::is_same<T, double>::value)                 draw_vec.push_back(std::stod(num));
                else if( std::is_same<T, float>::value)             draw_vec.push_back(std::stof(num));
                else if( std::is_same<T, unsigned int>::value)      draw_vec.push_back(static_cast<unsigned int>(std::stoul(num)));
                else if( std::is_same<T, unsigned long>::value)     draw_vec.push_back(std::stoul(num));
                else if( std::is_same<T, unsigned long long>::value)draw_vec.push_back(std::stoull(num));
                else if( std::is_same<T, int>::value)               draw_vec.push_back(std::stoi(num));
                else throw std::runtime_error("Unexpected type for read_bingo_input() received!");
            }
        }
        else 
        {
            throw std::runtime_error("No input found in file: " + file_path);
        }
        
        // Read Bingo tables and fill a vector of Bingo
        o_bingo_vec.clear();
        std::vector<T> bingo_in; 
        unsigned int lines_per_bingo{0u};
        while(getline(input_file, input_line))
        {
            if (input_line.empty()) // empty bingo_in
            {
                if (bingo_in.size() == static_cast<size_t>(BingoSize*BingoSize)) // create new bingo table 
                {
                    o_bingo_vec.push_back(Bingo<T, BingoSize>{bingo_in});
                }
                bingo_in.clear();
                lines_per_bingo = 0u;
                continue;
            }
            else // fill bingo_in vec
            {
                std::vector<T> new_line = split_string_in_num<unsigned int>(input_line, ' ');
                if (new_line.size() != BingoSize)
                {
                    std::string str = std::string("Error: Expected " + std::to_string(BingoSize) +  " values after splitting string, but received " + std::to_string(new_line.size()) + " values!");
                    throw std::runtime_error(str);
                }
                bingo_in.insert(bingo_in.end(), std::make_move_iterator(new_line.begin()), std::make_move_iterator(new_line.end()));
            }
            ++lines_per_bingo;
        }
        // Create last bingo
        if (bingo_in.size() == static_cast<size_t>(BingoSize*BingoSize)) // create new bingo table 
        {
            o_bingo_vec.push_back(Bingo<T, BingoSize>{bingo_in});
        }
        input_file.close();   //close the file object.
    }
    return draw_vec;
}

const unsigned int get_last_bingo_score(const std::string& bingo_input_file_path)
{
    std::vector<Bingo<unsigned int, 5u>> bingo_vec;
    std::vector<unsigned int> draws = read_bingo_input<unsigned int, 5u>(bingo_input_file_path, bingo_vec);
    for (const auto& val : draws)
    {
        for (auto it=bingo_vec.begin(); it !=bingo_vec.end(); )
        {
            it->mark_number(val);
            if(it->is_bingo())
            {
                if (bingo_vec.size() == 2) 
                    ;
                if (bingo_vec.size() == 1)
                {
                    return it->get_score();
                }
                bingo_vec.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }
    throw std::runtime_error("Reached end of get_last_bingo_score and no last bingo was found!");
}