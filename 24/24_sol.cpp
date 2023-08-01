#include <string>
#include <map>
#include <vector>
#include <array>
#include <limits>

#include "../utility.h"

/*
The only value that gets transferred between the multiple 
inp w instructions seems to be z. x and y are always set to 
0 and w contains the new value. 
Approach 1: 
    Find all pairs of carry 'z' and input 'w' for the last inp 
    instruction so z will be zero. 
    For each pair we can find possible solutions for the inp 
    instruction before (13th) so x will have the desired value
    required in the last instruction. This can be repeated up to the first instruction set
    -> did not terminate
Approach 2:
    There are 14 instruction sets overall and they can be divided in two actions:
    - Divide z by 26: z=z/26, if (z%26)-<some_val> == w
    - Multiply z by 26: z=26*z+w+<some_val> (irrespective of input w)
    Each instruction set where "div z 26" is contained can be used to reduce the search space
    (i.e., valid states) if we enforce the condition is fulfilled, that is (z%26)-<some_val> == w
    If this is not the case the multiply by 26 and divide by 26 actions would be unbalanced
    and z get never down to 0. Therefore, the assumption that all conditions must be fulfilled
    should be applicable
*/

// pair of (valio) w and z values that serve as input to a single
// digit check of the 14 digit model number
struct ValidWZPair{
    int w;
    int z;
};

// state containing the concatenated valid values for w
// and the z value for the next digit check
struct IntermediateState{
    uint64_t i_model_num{ 0 };
    int z{ 0 };
};

/**
 * @brief possible types of instructions
 * 
 */
enum class InsType{
    Input,
    Add, 
    Multiply,
    Divide, 
    Modulo, 
    Equal
};

// mapping between instruction string and instruction type
const std::map<std::string,InsType> type_map{   {"add", InsType::Add},
                                                {"mul", InsType::Multiply},
                                                {"div", InsType::Divide},
                                                {"mod", InsType::Modulo},
                                                {"eql", InsType::Equal}, 
                                                {"inp", InsType::Input}};

// structure for instruction representation in code
struct Instruction {
    InsType ins_type{ }; ///< type of instruction
    char a{ }; ///< char for the first operand
    std::string b{ }; ///< string for the second operand (can be an integer or char ['w','x','y','z'])

    bool operator==(const Instruction &ins) const{
        return ins_type==ins.ins_type && a==ins.a && b==ins.b;
    }
};

// class for executing the instructions
struct ALU{
    std::map<char, int> vars{ {'x',0}, {'y',0}, 
                              {'w',0}, {'z',0} };
    void do_instruction(char a, char b, InsType ins_type);
    void do_instruction(char a, int b, InsType ins_type);
};

//function declarations
/**
 * @brief Executes the instruction set ins_vec with a given z value and all
 * valid w values [1,...,9]
 * 
 * @param z start value for z before executing ins_vec
 * @param ins_vec vector of instructions
 * @return ValidWZPair pair of w and z, where w was used as input to get the corresponding z value
 */
std::vector<ValidWZPair> get_valid_z_values(int z, const std::vector<Instruction> &ins_vec);

/**
 * @brief read in instructions from data file
 * 
 * @param file_path 
 * @return std::vector<std::vector<Instruction>> 
 */
std::vector<std::vector<Instruction>> read_data(const std::string &file_path);


/**
 * @brief checks whether ins_vec contains a instruction dividing z by 26
 * 
 * @param ins_vec 
 * @return true, if ins_vec contains a instruction div z 26
 */
bool does_contain_divide_26(const std::vector<Instruction> &ins_vec);

/**
 * @brief Reduces the vector of valid states by only keeping those that fulfill the condition
 * stated in the ins_vec ((z%26)-<some_val> == w). <some_val> can be taken from the fourth
 * instruction of ins_vec
 * 
 * @param pos current index in overall model number
 * @param valid_states valid input states
 * @param ins_vec instructions
 * @return std::vector<IntermediateState> returns new valid states based on the condition requirement
 */
std::vector<IntermediateState> reduce_valid_states(int pos, std::vector<IntermediateState> &valid_states, const std::vector<Instruction> &ins_vec);

std::pair<std::uint64_t,std::uint64_t> day_24_1_2(const std::string &file_path)
{
    std::vector<std::vector<Instruction>> ins_vec = read_data(file_path);
    std::vector<std::vector<int>> valid_model_numbers;
    std::vector<IntermediateState> valid_states{ {{}, 0} };

    // for (const auto &ins_set : ins_vec)
    for (int pos=0;pos<14; ++pos)
    {
        auto &ins_set = ins_vec[pos];

        if (does_contain_divide_26(ins_set))
        {
            valid_states = reduce_valid_states(pos, valid_states, ins_set);
            continue;
        }

        std::vector<IntermediateState> new_valid_states;
        for (const auto &state : valid_states)
        {
            std::vector<ValidWZPair> valid_pairs = get_valid_z_values(state.z, ins_set);
            for (const auto &valid_pair : valid_pairs)
            {
                auto new_state{ state };
                new_state.i_model_num *= 10;
                new_state.i_model_num += valid_pair.w;
                new_state.z = valid_pair.z;
                new_valid_states.push_back(new_state);
            }
        }
        valid_states = new_valid_states;
        // --pos;
    }

    // z must be zero in the beginning -> filter out all invalid numbers requiring z != 0
    std::uint64_t largest_model_num{ 0 };
    std::uint64_t smallest_model_num{ std::numeric_limits<unsigned long long>::max() };
    for (const auto &valid_state : valid_states)
    {
        if (valid_state.i_model_num > largest_model_num)
        {
            largest_model_num = valid_state.i_model_num;
        }
        if (valid_state.i_model_num < smallest_model_num)
        {
            smallest_model_num = valid_state.i_model_num;
        }
    }

    return { smallest_model_num,largest_model_num };
}

std::vector<IntermediateState> reduce_valid_states(int pos, std::vector<IntermediateState> &valid_states, const std::vector<Instruction> &ins_vec)
{
    std::vector<IntermediateState> new_valid_states;

    // retrieve <Val> of "add x <Val>" in fourth instruction
    int val_x = std::stoi(ins_vec[4].b);

    for (auto &state : valid_states)
    {
        if (state.z%26+val_x < 10 && state.z%26+val_x > 0) 
        {
            auto tmp = state;
            tmp.z /= 26;
            tmp.i_model_num *= 10;
            tmp.i_model_num += state.z%26+val_x;
            new_valid_states.push_back(tmp);
        }
    }
    return new_valid_states;
}

std::vector<ValidWZPair> get_valid_z_values(int z, const std::vector<Instruction> &ins_vec)
{
    std::vector<ValidWZPair> valid_pairs;

    for (int w=1; w < 10; ++w)
    {
        ALU alu{};
        alu.vars['w'] = w;
        alu.vars['z'] = z;
        for (const auto &ins : ins_vec) 
        {
            if (ins.b.length() == 1 && ins.b[0] >= 'w' && ins.b[0] <= 'z')
            {
                alu.do_instruction(ins.a, ins.b[0], ins.ins_type);
            }
            else
            {
                int val_b = std::stoi(ins.b);
                alu.do_instruction(ins.a, val_b, ins.ins_type);
            }
        }
        valid_pairs.push_back({ w,alu.vars['z'] });
    }
    return valid_pairs;
}

bool does_contain_divide_26(const std::vector<Instruction> &ins_vec)
{
    const Instruction division26{InsType::Divide, 'z', "26"};
    for (const auto &ins : ins_vec)
    {
        if (division26 == ins) return true;
    }
    return false;
}


void ALU::do_instruction(char a, char b, InsType ins_type) {
    int b_int{ 0 };
    // distinguish between b being a number and b being a variable w,x,y,z
    if (auto res = vars.find(b); res != vars.end())
    {
        b_int = res->second;
    }
    else
    {
        if (!std::isdigit(static_cast<unsigned char>(b)))
        {
            throw std::invalid_argument("Trying to convert non-digit char to digit!");
        }
        b_int = b - '0';
    }
    do_instruction(a, b_int, ins_type);
}

void ALU::do_instruction(char a, int b, InsType ins_type) {

    switch(ins_type) {
        // case InsType::Input:
            
        //     break;
        case InsType::Add:
            vars.at(a) += b;
            break;
        case InsType::Multiply:
            vars.at(a) *= b;
            break;
        case InsType::Divide:
            vars.at(a) /= b;
            break;
        case InsType::Modulo:
            vars.at(a) %= b;
            break;
        case InsType::Equal:
            vars.at(a) = vars.at(a) == b ? 1 : 0;
            break;
        default:
            throw std::runtime_error("Unknown instruction type detected!");
    }
}

std::vector<std::vector<Instruction>> read_data(const std::string &file_path)
{
    std::vector<std::string> str_vec = read_string_vec_from_file(file_path);
    std::vector<std::vector<Instruction>> ins_vec;
    std::vector<Instruction> tmp_vec;

    for (const auto &str : str_vec)
    {
        std::vector<std::string> sub_str = split_string(str," ");
        InsType type = type_map.at(sub_str[0]);

        if (type == InsType::Input)
        {
            ins_vec.push_back({});
        }
        else
        {
            ins_vec.back().push_back(Instruction{ type, sub_str[1][0], sub_str[2]});
        }
    }
    return ins_vec;
}