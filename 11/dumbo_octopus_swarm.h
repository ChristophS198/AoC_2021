#include <vector>
#include <array>
#include <string>
#include <algorithm>


template<typename T, size_t ArraySize>
class DumboOctopusSwarm
{
    template<typename T2>
    using u_2d_field = std::array<std::array<T2, ArraySize>, ArraySize>;
public:
    DumboOctopusSwarm(u_2d_field<T> octopus_field);
    DumboOctopusSwarm(std::vector<std::vector<T>>&& octopus_field);

    unsigned int multi_step(unsigned int num_steps, bool debug_on);
    unsigned int find_synchro_step(bool debug_on);

    void print_dumbo_swarm() const;
private:
    unsigned int step();
    void execute_flash_rec(size_t row, size_t col);
    u_2d_field<T> m_octopus_field{};
    size_t m_dim{ArraySize};
};

template<typename T, size_t ArraySize>
DumboOctopusSwarm<T, ArraySize>::DumboOctopusSwarm(std::vector<std::vector<T>>&& octopus_field) : 
    m_dim{ArraySize}
{
    if (octopus_field.size() == 0 || octopus_field.size() != octopus_field[0].size())
    {
        throw std::invalid_argument("Constructor argument must be a 2D-vector with equal dimension (larger than 0)!");
    }
    for (size_t row=0; row < m_dim; ++row)
    {
        std::copy_n(octopus_field[row].begin(), m_dim, m_octopus_field[row].begin());
    }
}

template<typename T, size_t ArraySize>
DumboOctopusSwarm<T, ArraySize>::DumboOctopusSwarm(u_2d_field<T> octopus_field) : 
    m_octopus_field(octopus_field), m_dim{ArraySize}
{

}

template<typename T, size_t ArraySize>
unsigned int DumboOctopusSwarm<T, ArraySize>::find_synchro_step(bool debug_on)
{
    unsigned int num_steps{0};
    unsigned int num_flashes{0};
    while (num_flashes != m_dim*m_dim)
    {
        num_flashes = step();
        ++num_steps;
    }
    return num_steps;
}

template<typename T, size_t ArraySize>
unsigned int DumboOctopusSwarm<T, ArraySize>::multi_step(unsigned int num_steps, bool debug_on)
{
    unsigned int sum{0};
    for (size_t i=0; i<num_steps; ++i)
    {
        if (debug_on) 
        {
            print_dumbo_swarm();
        }
        sum += step();
    }
    if (debug_on)
    {
        print_dumbo_swarm();
    }
    return sum;
}

template<typename T, size_t ArraySize>
unsigned int DumboOctopusSwarm<T, ArraySize>::step()
{
    DumboOctopusSwarm::u_2d_field<bool> flash_field{}; // value-initialization with 0
    for (auto row=0u; row < m_dim; ++row) 
    {
        for (auto col=0u; col < m_dim; ++col)
        {
            ++m_octopus_field[row][col];
            if (m_octopus_field[row][col] == 10) 
            {
                flash_field[row][col] = true;
                execute_flash_rec(row, col);
            }
        }
    }
    // reset all entries that flashed in this step
    unsigned int num_flashes{0};
    for (auto& row : m_octopus_field) 
    {
        for (auto& elem : row)
        {
            if (elem > 9) 
            {
                ++num_flashes;
                elem = 0;
            }
        }
    }
    return num_flashes;
}

template<typename T, size_t ArraySize>
void DumboOctopusSwarm<T, ArraySize>::execute_flash_rec(size_t row, size_t col)
{
    bool is_leftmost  = col == 0;
    bool is_rightmost = col == m_dim - 1;
    bool is_bottom    = row == m_dim - 1;
    bool is_top       = row == 0;

    if (!is_leftmost) 
    {
        if (++m_octopus_field[row][col-1] == 10) execute_flash_rec(row, col-1);
        if (!is_bottom && ++m_octopus_field[row+1][col-1] == 10) execute_flash_rec(row+1, col-1);
        if (!is_top    && ++m_octopus_field[row-1][col-1] == 10) execute_flash_rec(row-1, col-1);
    }
    if (!is_rightmost) 
    {
        if (++m_octopus_field[row][col+1] == 10) execute_flash_rec(row, col+1);
        if (!is_bottom && ++m_octopus_field[row+1][col+1] == 10) execute_flash_rec(row+1, col+1);
        if (!is_top    && ++m_octopus_field[row-1][col+1] == 10) execute_flash_rec(row-1, col+1);
    }
    if (!is_bottom && ++m_octopus_field[row+1][col] == 10) execute_flash_rec(row+1, col);
    if (!is_top && ++m_octopus_field[row-1][col] == 10)    execute_flash_rec(row-1, col);
}

template<typename T, size_t ArraySize>
void DumboOctopusSwarm<T, ArraySize>::print_dumbo_swarm() const
{
    std::cout << "\n";
    for (const auto& row : m_octopus_field) 
    {
        for (const auto& elem : row)
        {
            if (elem == 0) 
            {
                std::cout << bold_on << elem << bold_off << " ";
            }
            else
            {
                std::cout << elem << " ";
            }
        }
        std::cout << "\n";
    }
}