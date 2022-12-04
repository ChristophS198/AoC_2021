#include <iostream>
#include <fstream>
#include <string>
#include <vector>

/**
 * @brief Read the measurement data of a radar sweap from file
 * 
 * @param file_path path to measurement file
 * @return measurements as vector of ints
 */
std::vector<int> get_measurement(const std::string& file_path);

/**
 * @brief iterate through vector and count the number of times consecutive entries increase
 * 
 * @param meas_vec measurement vector 
 * @return unsigned int. Number of increases in measurement data
 */
unsigned int count_increases(const std::vector<int>& meas_vec);

/**
 * @brief slide a window of size win_size over the input vector and create a new vector containing sum of win_size consecutive entries
 * 
 * @tparam T datatype of vector
 * @param win_size size of sliding window
 * @param vec_in input vector containing measurement data
 * @return std::vector<T> reduced output vector where consecutive elements are summed up through sliding window
 */
template<typename T>
std::vector<T> sliding_win_measurement(const unsigned int win_size, const std::vector<T>& vec_in);


template<typename T>
std::vector<T> sliding_win_measurement(const unsigned int win_size, const std::vector<T>& vec_in)
{
    if (win_size == 1) 
    {
        return vec_in;
    }
    if (vec_in.size() == 0)
    {
        return vec_in;
    }

    // verify win_size is not larger than vector size
    unsigned int lim_win_size = win_size < static_cast<unsigned int>(vec_in.size()) ? win_size : vec_in.size();
    size_t out_len = vec_in.size()-lim_win_size+1;
    std::vector<T> vec_out(out_len, T{});
    
    // slide window over vector
    T sum{0};
    for(size_t i=0; i<vec_in.size(); ++i)
    {
        sum += vec_in[i]; // add current element
        if (i+1 >= lim_win_size) // if window is full start filling output vector
        {
            size_t j = i - lim_win_size + 1; // index of output element to be filled 
            vec_out[j] = sum;
            sum -= vec_in[j]; // subtract element that falls out of window in next iteration 
        }
    }

    return vec_out;
}

unsigned int count_increases(const std::vector<int>& meas_vec)
{
    unsigned int num_inc{0};
    if (meas_vec.size() == 0) 
    {
        return num_inc;
    }
    for(size_t i=0; i < meas_vec.size()-1; ++i) 
    {
        if(meas_vec[i+1] > meas_vec[i]) ++num_inc;
    }
    return num_inc;
}

std::vector<int> get_measurement(const std::string& file_path)
{
    std::vector<int> meas_vec{};
    std::fstream input_file;
    input_file.open(file_path,std::ios::in); //open a file to perform read operation using file object
    if (input_file.is_open()){   //checking whether the file is open
        std::string tp;
        while(getline(input_file, tp)){  //read data from file object and put it into string.
        meas_vec.push_back(stoi(tp));
        }
        input_file.close();   //close the file object.
    }
    return meas_vec;
}