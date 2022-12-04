#include <vector>
#include <fstream>
#include <numeric>

class LanternFishPopulation 
{
public:
    LanternFishPopulation(const std::vector<unsigned int>& init_values, unsigned int spawn_value, unsigned int first_spawn_value);
    
    /**
     * @brief decreases the timer value of each lanternfish by 1
     * When the timer of a lanternfish would become negative it spawns a new 
     * lanternfish and resets its timer
     */
    void decrease_timer_value();

    /**
     * @brief executes multiple decreases 
     * 
     * @param days number of days that are simulated
     */
    void simulate_multiple_days(unsigned int days, std::ostream& out=std::cout, bool debug=false);

    /**
     * @brief Get the population size 
     * 
     * @return unsigned int 
     */
    uint64_t get_population_size() const;

    std::ostream& print_population(std::ostream& out) const;


private:

    /**
     * @brief Get the population size 
     * 
     * @return unsigned int 
     */
    unsigned int get_population_size(const std::vector<uint64_t>& population) const;

    unsigned int m_spawn_value{6}; ///< when the timer of an existing lanternfish would become negative it spawns a new lanternfish and resets its timer to this value
    unsigned int m_first_spawn_value{8}; ///< after a new lanternfish is created its timer is set to this value
    std::vector<uint64_t> m_fish_numbers{}; ///< this vector counts for each valid timer value (vector index) how many fish have this value  
};

LanternFishPopulation::LanternFishPopulation(const std::vector<unsigned int>& init_values, unsigned int spawn_value, unsigned int first_spawn_value) 
: m_spawn_value{spawn_value}, m_first_spawn_value{first_spawn_value}, m_fish_numbers(first_spawn_value,0u)
{
    // Count how many fish exist for each valid timer value
    for (const auto& fish : init_values)
    {
        ++m_fish_numbers[fish];
    }
}


uint64_t LanternFishPopulation::get_population_size() const 
{ 
    return std::accumulate(m_fish_numbers.begin(), m_fish_numbers.end(), 0l);
}


void LanternFishPopulation::simulate_multiple_days(unsigned int days, std::ostream& out, bool debug)
{
    for (unsigned int i=1u; i <= days; ++i)
    {
        decrease_timer_value();
        if (debug)
        {
            out << "After " << i << " Days: ";
            print_population(out);
        }
    }
}


void LanternFishPopulation::decrease_timer_value()
{
    std::vector<uint64_t> new_population(m_fish_numbers.size());
    for (size_t i=1; i!=m_fish_numbers.size(); ++i)
    {
        new_population[i-1] = m_fish_numbers[i];
    }
    new_population[m_first_spawn_value-1] = m_fish_numbers[0]; ///< Accouts for all newly spawned fish
    new_population[m_spawn_value-1] += m_fish_numbers[0]; ///< And reset all timers for fish that currently spawned a new fish
    m_fish_numbers = new_population;
}

std::ostream& LanternFishPopulation::print_population(std::ostream& out) const
{
    out << "Number of fish: " << std::accumulate(m_fish_numbers.begin(), m_fish_numbers.end(), 0l) << "\n";
    return out;
}
