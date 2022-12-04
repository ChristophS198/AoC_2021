#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <set>

#include"../utility.h"

struct SignalEntry
{
    std::vector<std::string> signal_pattern{}; ///< Stores patterns for all 10 unique signals [0, 1, ..., 9]
    std::vector<std::string> output_value{}; ///< Stores the 4 digit output value
};

// function declarations
SignalEntry parse_string_to_SignalEntry(const std::string& line);
std::vector<SignalEntry> read_patterns_from_file(const std::string& file_path);
std::map<unsigned int, unsigned int> find_digit_codes(const SignalEntry& entry);
unsigned int decode_output(std::map<unsigned int, unsigned int> digit_map, const std::vector<std::string>& output_values);
unsigned int encode_string(const std::string& str);

unsigned int sum_decoded_output_values(const std::string& file_path)
{
    std::vector<SignalEntry> signal_entries = read_patterns_from_file(file_path);
    std::vector<unsigned int> decoded_output(signal_entries.size(),0u);
    for (const auto& entry : signal_entries)
    {
        std::map<unsigned int, unsigned int> digit_map = find_digit_codes(entry);
        decoded_output.push_back(decode_output(digit_map, entry.output_value));
    }
    return std::accumulate(decoded_output.begin(), decoded_output.end(), 0u);
}

unsigned int num_unique_numbers(const std::string& file_path)
{
    std::vector<SignalEntry> signal_entries = read_patterns_from_file(file_path);
    unsigned int count{0};
    for (const auto& entry : signal_entries)
    {
        for (const auto& val : entry.output_value)
        {
            if (val.size() < 5 || val.size() == 7) // detect digits 1,4,7,8 based on segments (2,3,4 or 7)
            {
                ++count;
            }
        }
    }
    return count;
}


/**
 * @brief Parses a single entry to a SignalEntry struct
 * 
 * @param line single entry line (ten patterns | four output values) 
 * @return SignalEntry 
 */
SignalEntry parse_string_to_SignalEntry(const std::string& line)
{
    SignalEntry entry;
    const std::string delim(" | "); // delimiter between patterns and output values
    size_t pos = line.find(delim); 
    if (pos == std::string::npos) 
    {
        std::string str = std::string("Line \n") + line + std::string("\ndoes not match expected Syntax: 'ten patterns | four outupt values'");
        throw std::runtime_error(str);
    }
    std::string pattern = line.substr( 0, pos );
    std::string output = line.substr(pos+delim.length(), line.length());
    entry.signal_pattern = split_string(pattern, " ");
    entry.output_value = split_string(output, " ");
    if (entry.signal_pattern.size() != 10 || entry.output_value.size() != 4)
    {
        std::string str = std::string("Line \n") + line + std::string("\ndoes not contain expected number of patterns: 'ten patterns | four outupt values'");
        throw std::runtime_error(str);
    }
    return entry;
}

std::vector<SignalEntry> read_patterns_from_file(const std::string& file_path)
{
    std::vector<SignalEntry> pattern_vec{};
    std::fstream input_file;
    input_file.open(file_path,std::ios::in);
    if (input_file.is_open()){
        std::string input_line;
        while(getline(input_file, input_line)){  //read data from file object and put it into string.
            pattern_vec.push_back(parse_string_to_SignalEntry(input_line));
        }
        input_file.close();   //close the file object.
    }
    return pattern_vec;
}

/**
 * @brief Checks if all characters of substr are contained in str 
 * 
 * @param str 
 * @param substr 
 * @return true all characters of substr are found in str - else false
 */
bool is_subset(const std::string& str, const std::string& substr)
{
    for (const auto& c : substr)
    {
        if(str.find(c) == std::string::npos)
        {
            return false;
        }
    }
    return true;
}

/**
 * @brief Returns a string containing all characters of str1 that are not found in str2
 * 
 * @param str1
 * @param str2 
 * @return a string containing all characters of str1 that are not found in str2
 */
std::string char_diff(const std::string& str1, const std::string& str2)
{
    std::string diff{};
    for (const auto& c : str1)
    {
        if(str2.find(c) == std::string::npos)
        {
            diff.push_back(c);
        }
    }
    return diff;
}

/**
 * @brief checks if both strings are built using the same characters, with the help of sets
 * 
 * @param str1 
 * @param str2 
 * @return true if both strings contain the same characters (ordering is not relevant)
 */
bool contain_same_chars(const std::string& str1, const std::string& str2)
{
    return std::set<char>(str1.begin(), str1.end())
        == std::set<char>(str2.begin(), str2.end());
}

std::map<unsigned int, unsigned int> find_digit_codes(const SignalEntry& entry)
{
    std::map<unsigned int, unsigned int> digit_map; ///< 
    std::map<unsigned int, std::vector<std::string>> len_map; ///< 
    std::vector<std::string> digit_vec(10); ///< index corresponds to digit, entry to signal representation: 0: Segments for digit 0, 1: ...
    for (const auto& val : entry.signal_pattern)
    {
        len_map[val.size()].push_back(val);
    }
    // Based on length 1,4,7,8 are known
    digit_map[encode_string(len_map[2][0])] = 1u;
    digit_map[encode_string(len_map[4][0])] = 4u;
    digit_map[encode_string(len_map[3][0])] = 7u;
    digit_map[encode_string(len_map[7][0])] = 8u;
    digit_vec[1] = len_map[2][0];
    digit_vec[4] = len_map[4][0];
    digit_vec[7] = len_map[3][0];
    digit_vec[8] = len_map[7][0];
    
    // digit 3 has length 5 and contains the set of digit 1
    for (auto it = len_map[5].begin(); it < len_map[5].end(); ++it)
    {
        if (is_subset(*it, digit_vec[1]))
        {
            digit_map[encode_string(*it)] = 3u;
            digit_vec[3] = *it;
            len_map[5].erase(it);
        }
    }

    // digits 0,6,9 have length 6
    // if digit contains subset 3 -> 9, elif contains 1 -> 0, else 6
    for (auto it = len_map[6].begin(); it < len_map[6].end(); ++it)
    {
        if (is_subset(*it, digit_vec[3]))
        {
            digit_map[encode_string(*it)] = 9;
            digit_vec[9] = *it;
        }
        else
        {
            if(is_subset(*it, digit_vec[1]))
            {
                digit_map[encode_string(*it)] = 0;
                digit_vec[0] = *it;
            }
            else 
            {
                digit_map[encode_string(*it)] = 6;
                digit_vec[6] = *it;
            }
        }
    }

    // digit 5 has length 5 and has a segment diff of 1 to digit 9
    // digit 2 has length 5 and does not contain the set
    for (auto it = len_map[5].begin(); it < len_map[5].end(); ++it)
    {
        std::string diff = char_diff(digit_vec[9], *it);
        if (diff.length() == 1)
        {
            digit_map[encode_string(*it)] = 5;
            digit_vec[5] = *it;
        }
        else
        {
            digit_map[encode_string(*it)] = 2;
            digit_vec[2] = *it;
        }
    }
    return digit_map;
}


unsigned int decode_output(std::map<unsigned int, unsigned int> digit_map, const std::vector<std::string>& output_values)
{
    unsigned int decoded_value{0u};
    for (const auto& val : output_values)
    {
        decoded_value = 10*decoded_value + digit_map[encode_string(val)];
    }
    return decoded_value;
}

/**
 * @brief Uses a unique unsigned value to represent a string that only contains chars (a, b, ..., g)
 * This allows an encoding that does not depend on the character order
 * 
 * @param str 
 * @return unsigned int 
 */
unsigned int encode_string(const std::string& str)
{
    unsigned int sum{0};
    for (const auto& c : str)
    {
        sum += 1 << static_cast<int>(c-'a');
    }
    return sum;
}