#include <array>
#include <vector>
#include <algorithm>

template <typename T, std::size_t ArraySize>
class Bingo final
{

public:
    Bingo() = default;
    Bingo(const std::array<std::array<T, ArraySize>, ArraySize>& values);
    Bingo(const std::vector<T>& values);
    Bingo(std::vector<T>&& values);

    /**
     * @brief checks if the drawn number (num) exists in m_values and marks all occurrences if it does exist
     * 
     * @param num number that was drawn 
     */
    void mark_number(const T& num);

    /**
     * @brief checks if all elements of a row or column are marked
     * 
     * @return true if all elements of a row or column are marked
     * @return false otherwise
     */
    bool is_bingo() const;

    /**
     * @brief Get the score by summing up all unmarked fields and multiplying the sum with the last draw (m_last_draw)
     * 
     * @return const T score value
     */
    const T get_score() const; 

    /**
     * @brief print bingo content to stream
     * 
     */
    std::ostream& print_bingo(std::ostream& out) const;

private:
    size_t m_dim = ArraySize; ///< Dimension of m_values 
    std::array<std::array<T, ArraySize>, ArraySize> m_values{}; ///< 2D-array which is 0 initialized and later contains the random values of a Bingo field
    std::array<std::array<bool, ArraySize>, ArraySize> m_marked{}; ///< 2D-array of same dimensions as m_values, where every value is 0 after init and a 1 is inserted if corresponding m_values value is marked
    T m_last_draw{0}; ///< stores the value of the last draw, so we can calculate the winning score
};


template<typename T, std::size_t ArraySize>
const T Bingo<T, ArraySize>::get_score() const
{
    T sum{0};
    for (size_t row=0; row<m_dim; ++row) 
    {
        for (size_t col=0; col<m_dim; ++col)
        {
            sum += (!m_marked[row][col]) * m_values[row][col];
        }
    }
    return sum * m_last_draw;
}



template <typename T, std::size_t ArraySize>
Bingo<T, ArraySize>::Bingo(const std::array<std::array<T, ArraySize>, ArraySize>& values) : m_dim{ArraySize}
{
    m_values = values;
}


template <typename T, std::size_t ArraySize>
Bingo<T, ArraySize>::Bingo(const std::vector<T>& values) : m_dim{ArraySize}
{
    if (values.size() != m_dim*m_dim) 
    {
        std::string str = std::string("Size of std::vector (") + std::to_string(values.size()) + std::string(") must be squared value of Bingo size: ") + std::to_string(ArraySize);
        throw std::invalid_argument(str);
    }
    for (size_t i=0; i<values.size(); ++i)
    {
        m_values[i/m_dim][i%m_dim] = values[i];
    }
}

template <typename T, std::size_t ArraySize>
Bingo<T, ArraySize>::Bingo(std::vector<T>&& values)
{
    if (values.size() != m_dim*m_dim) 
    {
        std::string str = std::string("Size of std::vector (") + std::to_string(values.size()) + std::string(") must be squared value of Bingo size: ") + std::to_string(ArraySize);
        throw std::invalid_argument(str);
    }
    for (size_t i=0; i<values.size(); ++i)
    {
        m_values[i/m_dim][i%m_dim] = values[i];
    }
}

template <typename T, std::size_t ArraySize>
void Bingo<T, ArraySize>::mark_number(const T& num)
{
    m_last_draw = num;
    for (size_t row=0; row < m_dim; ++row)
    {
        for (size_t col=0; col < m_dim; ++col)
        {
            if (m_values[row][col] == num)
            {
                m_marked[row][col] = true;
            }
        }
    }
}

template <typename T, std::size_t ArraySize>
bool Bingo<T, ArraySize>::is_bingo() const 
{
    std::vector<bool> col_marked(m_dim, true); 
    for (size_t row=0; row < m_dim; ++row)
    {
        bool row_marked{1};
        for (size_t col=0; col < m_dim; ++col)
        {
            col_marked[col] = col_marked[col] & m_marked[row][col];
            row_marked &= m_marked[row][col];
        }
        if (row_marked)
        {
            return true;
        }
    }
    if (std::find(col_marked.begin(), col_marked.end(), true) != col_marked.end())
    {
        return true;
    }
    return false;
}


template <typename T, std::size_t ArraySize>
std::ostream& Bingo<T, ArraySize>::print_bingo(std::ostream& out) const
{
    out << "\n";
    for (size_t row=0; row<m_dim; ++row) 
    {
        for (const auto& val : m_values[row])
        {
            out << val << " ";
        }
        out << "\t";
        for (const auto& mark : m_marked[row])
        {
            out << mark << " ";
        }
        out << "\n";
    }

    return out;
}
