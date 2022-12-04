#include <string>
#include <fstream>

#include "Dies.h"
#include "../utility.h"


struct StartPos
{
    Position pos_player_1;
    Position pos_player_2;
};

struct Player
{
    Position pos;
    std::uint64_t score;
};

//function declarations
StartPos get_start_pos(const std::string &file_path);
Position advance_track(const Position start, const std::uint64_t steps);

std::uint64_t day_21_1(const std::string &file_path)
{
    StartPos start_pos = get_start_pos(file_path);

    DetDice det_dice{1u, 100u};
    std::uint64_t num_rolls{ 0 };
    Player player_1{start_pos.pos_player_1, 0u};
    Player player_2{start_pos.pos_player_2, 0u};

    while (true)
    {
        std::uint64_t dice_sum = det_dice.roll_dice_thrice();
        player_1.pos = advance_track(player_1.pos ,dice_sum);
        player_1.score += player_1.pos;
        num_rolls += 3; 
        if (player_1.score > 999u)
        {         
            return num_rolls * player_2.score;
        }
        dice_sum = det_dice.roll_dice_thrice();
        player_2.pos = advance_track(player_2.pos ,dice_sum);
        player_2.score += player_2.pos;
        num_rolls += 3; 
        if (player_2.score > 999u)
        {
            return num_rolls * player_1.score;
        }
    }
}

std::uint64_t day_21_2(const std::string &file_path)
{
    StartPos start_pos = get_start_pos(file_path);

    QuantumDieController die{start_pos.pos_player_1, start_pos.pos_player_2, 21u};
    
    std::uint64_t num_wins = die.play_game();

    return num_wins;
}

Position advance_track(const Position start, const std::uint64_t steps)
{
    Position new_pos = (static_cast<std::uint64_t>(start) + steps) % (MAX_SPACE_VAL);
    if (new_pos == 0)
    {
        return MAX_SPACE_VAL;
    }
    else
    {
        return new_pos;
    }
}


StartPos get_start_pos(const std::string &file_path)
{
    StartPos starting_pos;
    std::fstream input_file;
    input_file.open(file_path,std::ios::in);
    if (input_file.is_open()){
        std::string input_line;
        getline(input_file, input_line);
        starting_pos.pos_player_1 = get_number_after_str<std::uint8_t>(input_line, "position:");
        getline(input_file, input_line);
        starting_pos.pos_player_2 = get_number_after_str<std::uint8_t>(input_line, "position:");

        input_file.close();   //close the file object.
    }
    return starting_pos;
}
