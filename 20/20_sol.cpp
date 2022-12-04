#include <vector>
#include <string>
#include <fstream>

#include "../utility.h"

constexpr size_t BORDER_EXT = 4u; ///< Denotes how many rows and columns are prepended/appended before/after the input data
using Image = std::vector<std::vector<bool>>;

struct sAlgoData
{
    std::vector<bool> enhancement_algo;
    Image image_data;
};

// function declarations
std::uint32_t day_20_1(const std::string &file_path);
sAlgoData get_input_image(const std::string &file_path);
std::vector<bool> parse_string_to_bool_vec(const std::string &str_in, size_t ext_num=0);
sAlgoData calc_output_image(const sAlgoData &image_in);
std::uint32_t num_lit_pixels(const Image &image);

// debug functions
void print_image(std::ostream &out, const Image image);


std::uint32_t day_20_1(const std::string &file_path)
{
    sAlgoData input_data = get_input_image(file_path);

    sAlgoData output_data = calc_output_image(input_data);
    sAlgoData output_data_2 = calc_output_image(output_data);

    return num_lit_pixels(output_data_2.image_data);
}

std::uint32_t day_20_2(const std::string &file_path)
{
    sAlgoData input_data = get_input_image(file_path);
    for (size_t i=0; i<50; ++i) 
    {
        input_data = calc_output_image(input_data);
    }
    // too low: 5938
    return num_lit_pixels(input_data.image_data);
}

sAlgoData calc_output_image(const sAlgoData &algo_data_in)
{
    sAlgoData data_out;
    data_out.enhancement_algo = algo_data_in.enhancement_algo;
    const Image &image_in = algo_data_in.image_data;
    if (!image_in[0][0] && data_out.enhancement_algo[0])
    {
        data_out.image_data.resize(image_in.size()+2, std::vector<bool>(image_in[0].size()+2, true));
    }
    else 
    {
        data_out.image_data.resize(image_in.size()+2, std::vector<bool>(image_in[0].size()+2, false));
    }
    Image &image_out = data_out.image_data;

    for (size_t row=1; row<image_in.size()-1; ++row)
    {
        for (size_t col=1; col<image_in[row].size()-1; ++col)
        {
            std::uint16_t algo_val{ 0 };
            algo_val |= static_cast<std::uint16_t>(image_in[row-1][col-1]);
            algo_val <<= 1;
            algo_val |= static_cast<std::uint16_t>(image_in[row-1][col]);
            algo_val <<= 1;
            algo_val |= static_cast<std::uint16_t>(image_in[row-1][col+1]);
            algo_val <<= 1;
            algo_val |= static_cast<std::uint16_t>(image_in[row][col-1]);
            algo_val <<= 1;
            algo_val |= static_cast<std::uint16_t>(image_in[row][col]);
            algo_val <<= 1;
            algo_val |= static_cast<std::uint16_t>(image_in[row][col+1]);
            algo_val <<= 1;
            algo_val |= static_cast<std::uint16_t>(image_in[row+1][col-1]);
            algo_val <<= 1;
            algo_val |= static_cast<std::uint16_t>(image_in[row+1][col]);
            algo_val <<= 1;
            algo_val |= static_cast<std::uint16_t>(image_in[row+1][col+1]);

            image_out[row+1][col+1] = algo_data_in.enhancement_algo[algo_val];
        }
    }
    return data_out;
}


void print_image(std::ostream &out, const Image image)
{
    for (const auto &row : image) 
    {
        for (const auto &elem : row)
        {
            char c = elem ? '#' : '.';
            out << c;
        }
    out << "\n";
    }
    out << "\n";
}

/**
 * @brief Get the input image object. The input image is extended by 1 in each dimension to 
 * incorporate the dark spots (0) bordering the scanned area
 * 
 * @param file_path 
 * @return sAlgoData 
 */
sAlgoData get_input_image(const std::string &file_path)
{
    sAlgoData input_data;
    std::fstream input_file;
    input_file.open(file_path,std::ios::in);
    if (input_file.is_open()){
        std::string input_line{};
        getline(input_file, input_line); // First line corresponds to enhancement string
        input_data.enhancement_algo = parse_string_to_bool_vec(input_line, 0u);
        getline(input_file, input_line); // Second line is empty
        if (input_line != "")
        {
            throw std::runtime_error("Unexpected format for input data!");
        }
        // starting from line 3 the input image data is read
        size_t row{ 0 };
        int cols{ -1 };
        while(getline(input_file, input_line)) 
        {  
            if (-1 == cols)
            {
                std::vector<bool> first_row = parse_string_to_bool_vec(input_line, BORDER_EXT);
                cols = first_row.size();
                for (size_t i=0u; i<BORDER_EXT; ++i)
                {
                    input_data.image_data.push_back(std::vector<bool>(cols, false));
                }
            }
            input_data.image_data.push_back(parse_string_to_bool_vec(input_line, BORDER_EXT));
            ++row;
        }
        for (size_t i=0u; i<BORDER_EXT; ++i)
        {
            input_data.image_data.push_back(std::vector<bool>(cols, false));
        }

        input_file.close();   //close the file object.
    }
    return input_data;
}


/**
 * @brief Parses a string of ',' and '#' to a bool vector ('.' == 0, '#' == 1)
 * 
 * @param str_in 
 * @param ext_num ext_num 0 values are prepended and appended to the resulting vector
 * @return std::vector<bool> 
 */
std::vector<bool> parse_string_to_bool_vec(const std::string &str_in, size_t ext_num)
{
    std::vector<bool> bool_vec(str_in.length()+2*ext_num, false); // first and last pixel are dark (false)
    size_t idx{ ext_num };

    for (const auto &c : str_in)
    {
        bool_vec[idx++] = c == '#' ? true : false;
    }
    return bool_vec;
}

std::uint32_t num_lit_pixels(const Image &image)
{
    std::uint32_t num_lit_pix{ 0 };
    for (const auto &row : image) 
    {
        for (const auto &elem : row)
        {
            if (elem)
            {
                ++num_lit_pix;
            }
        }
    }
    return num_lit_pix;
}
