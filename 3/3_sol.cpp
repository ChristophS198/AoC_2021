#include <string>
#include <fstream>
#include <vector>
#include <iostream>


/**
 * @brief converts a vector of unsigned ints, which contains bits, to its decimal representation
 * 
 * @param bits_vec vector of bits
 * @return const unsigned long long decimal representation of bits_vec
 */
const unsigned long long vec_to_dec(const std::vector<unsigned int>& bits_vec)
{
    unsigned long long decimal_rep{0};
    for(const unsigned int cur_bit : bits_vec)
    {
        decimal_rep = (decimal_rep << 1) | cur_bit; 
    }
    return decimal_rep;
}

/**
 * @brief Get the most common bits of each column in the input vector (vector of strings of same length with only ['0','1'])
 * 
 * @param bits_vec vector of strings where each string contains a concatenation of bits ['0','1']
 * @return std::vector<unsigned int> returns vector of most common bits. Either 0 if more bits at corresponding position of bits_vec entries contain more '0' than '1' else returns 1
 */
const std::vector<unsigned int> get_most_common_bits(const std::vector<std::string>& bits_vec)
{
    if (bits_vec.size() == 0)
    {
        return std::vector<unsigned int>{0};
    }
    const size_t bits_len = bits_vec.at(0).length();

    std::vector<unsigned int> ctr_0(bits_len,0); // this vector counts the '0' entries for each char position in bits_vec elements 
    for (const auto& bits : bits_vec)
    {
        if (bits.length() != bits_len) 
        {
            throw std::runtime_error("Error: Bits per line in diagnostic report are changing!");
        }
        for (size_t i=0; i<ctr_0.size(); ++i)
        {
            if (bits.at(i) == '0') 
            {
                ++(ctr_0.at(i));
            }
        }
    }
    // check if number of '0' entries is larger than half the number of diagnostic reports
    for (size_t i=0; i<ctr_0.size(); ++i)
    {
        unsigned int threshold = static_cast<unsigned int>(bits_vec.size() * 0.5f);
        if (ctr_0.at(i) > threshold) 
        {
            ctr_0.at(i) = 0;
        }
        else 
        {
            ctr_0.at(i) = 1;
        }
    }    
    return ctr_0;
}

/**
 * @brief Get the binary source for epsilon and gamma calculation from input file (diagnostic report)
 * 
 * @param file_path path to input file
 * @return const std::vector<std::string> vector of strings containing bits ['0','1']
 */
const std::vector<std::string> get_diagnostic_input(const std::string& file_path)
{
    std::vector<std::string> bit_vec{};
    std::fstream input_file;
    input_file.open(file_path,std::ios::in);
    if (input_file.is_open()){
        std::string bits_in_line;
        while(getline(input_file, bits_in_line)){  //read data from file object and put it into string.
            bit_vec.push_back(bits_in_line);
        }
        input_file.close();   //close the file object.
    }
    return bit_vec;
}


/**
 * @brief Solution to 3.1: Get the power consumption of the submarine based on diagnostic report 
 * 
 * @param file_path path to diagnostic report
 * @return const unsigned int power consumption as product of gamma and epsilon rate
 */
const unsigned long long get_power_consumption(const std::string& file_path)
{
    std::vector<std::string> diag_msgs = get_diagnostic_input(file_path);
    std::vector<unsigned int> most_common_bits = get_most_common_bits(diag_msgs);

    const unsigned long long gamma_rate = vec_to_dec(most_common_bits);

    std::vector<unsigned int> most_common_bits_inv;
    for (size_t i=0; i<most_common_bits.size(); ++i) {
        most_common_bits_inv.push_back(most_common_bits[i]^1);
    }
    const unsigned long long epsilon_rate = vec_to_dec(most_common_bits_inv);
    std::cout << "Gamma rate= " << gamma_rate << "\nepsilon rate = " << epsilon_rate << std::endl;
    return gamma_rate * epsilon_rate;
}




// ------------------------------- Solution to second part of Day 3 --------------------------------

/**
 * @brief Debug function to help analyse the recursive behaviour
 * 
 * @param file_path 
 * @param diag_msgs 
 */
void write_vec_to_file(const std::string& file_path, const std::vector<std::string>& diag_msgs)
{
    std::ofstream out(file_path);
    for (const auto& msg : diag_msgs) 
    {
        out << msg << "\n";
    }
    out.close();
}

/**
 * @brief Get the most common bit of one column in the input vector (vector of strings of same length with only ['0','1'])
 * 
 * @param bits_vec vector of strings where each string contains a concatenation of bits ['0','1']
 * @param bit_pos current bit position (column) that is analysed
 * @return unsigned int returns either 0 if more bits at position bit_pos contain '0' than '1' else returns 1
 */
const unsigned int get_most_common_bit(const std::vector<std::string>& bits_vec, const size_t bit_pos)
{
    if (bits_vec.size() == 0)
    {
        return 0u;
    }
    const size_t bits_len = bits_vec.at(0).length();
    if (bits_len < bit_pos+1)
    {
        throw std::runtime_error("get_most_common_bit is called with a larger bit position than there are bits in the bit array!" + std::to_string(bit_pos) + "+1>" + std::to_string(bits_len));
    }

    unsigned int ctr_0{0}; // counts the '0' entries at position bit_pos
    for (const auto& bits : bits_vec)
    {
        if (bits.length() != bits_len) 
        {
            throw std::runtime_error("Error: Bits per line in diagnostic report are changing!");
        }
        if (bits.at(bit_pos) == '0') 
        {
            ++ctr_0;
        }
    }
    // check if number of '0' entries is larger than half the number of diagnostic reports
    unsigned int threshold = static_cast<unsigned int>(bits_vec.size() * 0.5f);
    if (ctr_0 > threshold) 
    {
        ctr_0 = 0u;
    }
    else 
    {
        ctr_0 = 1u;
    }
    return ctr_0;
}


std::vector<std::string> truncate_diag_msgs(const std::vector<std::string>& diag_msgs, const size_t bit_pos, const unsigned int most_com_bit)
{
    std::vector<std::string> new_diags;
    const char ch = most_com_bit == 0 ? '0' : '1';
    for (const auto& diag : diag_msgs)
    {
        if (diag.at(bit_pos) == ch) 
        {
            new_diags.push_back(diag);
        }
    }
    return new_diags;
}

std::string get_oxygen_generator_val(const std::vector<std::string>& diag_msgs, const size_t bit_pos)
{
    if(diag_msgs.size() == 0)
    {
        throw std::invalid_argument("Error: get_oxygen_generator_val called with empty vector diag_msgs!");
    }
    if (diag_msgs.size() == 1) 
    {
        return diag_msgs[0];
    }
    else
    {
        unsigned int most_com_bit = get_most_common_bit(diag_msgs, bit_pos);
        std::vector<std::string> reduced_diag_msgs = truncate_diag_msgs(diag_msgs, bit_pos, most_com_bit);
        return get_oxygen_generator_val(reduced_diag_msgs, bit_pos+1);
    }
}

std::string get_CO2_scrubber_rating(const std::vector<std::string>& diag_msgs, const size_t bit_pos)
{
    if(diag_msgs.size() == 0)
    {
        throw std::invalid_argument("Error: get_CO2_scrubber_rating called with empty vector diag_msgs!");
    }
    if (diag_msgs.size() == 1) 
    {
        return diag_msgs[0];
    }
    else
    {
        // get least common bit by inverting result of get_most_common_bit
        unsigned int least_com_bit = get_most_common_bit(diag_msgs, bit_pos) ^ 1;
        std::vector<std::string> reduced_diag_msgs = truncate_diag_msgs(diag_msgs, bit_pos, least_com_bit);
        const std::string file_name = std::to_string(bit_pos) + std::string("_scrubber");
        // write_vec_to_file(file_name, reduced_diag_msgs);
        return get_CO2_scrubber_rating(reduced_diag_msgs, bit_pos+1);
    }
}


/**
 * @brief converts a string, which contains bits, to its decimal representation
 * 
 * @param bits_str vector of bits
 * @return const unsigned long long decimal representation of bits_vec
 */
const unsigned long long str_to_dec(const std::string& bits_str)
{
    unsigned long long decimal_rep{0};
    for(const auto& cur_char : bits_str)
    {
        unsigned int cur_bit = cur_char == '0' ? 0u : 1u;
        decimal_rep = (decimal_rep << 1) | cur_bit; 
    }
    return decimal_rep;
}

unsigned long long get_oxygen_generator_val(const std::string& file_path)
{
    std::vector<std::string> diag_msgs = get_diagnostic_input(file_path);
    const std::string oxygen_generator_str = get_oxygen_generator_val(diag_msgs,0);
    std::cout << "Oxygen Generator String: " << oxygen_generator_str << std::endl;
    const std::string co2_scrubber_rating_str = get_CO2_scrubber_rating(diag_msgs, 0);
    std::cout << "CO2 Scrubber Rating String: " << co2_scrubber_rating_str << std::endl;
    const unsigned long long oxygen_generator_dec = str_to_dec(oxygen_generator_str);
    const unsigned long long co2_scrubber_rating_dec = str_to_dec(co2_scrubber_rating_str);
    std::cout << "Oxygen Generator value: " << oxygen_generator_dec << std::endl;
    std::cout << "CO2 Scrubber Rating value: " << co2_scrubber_rating_dec << std::endl;

    return oxygen_generator_dec * co2_scrubber_rating_dec;
}
