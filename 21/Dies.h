#include <map>
#include <vector>
#include <set>
#include <iomanip>

constexpr std::uint8_t MAX_SPACE_VAL = 10u;

using Position = std::uint8_t;

struct ThrowEvalSinglePlayer
{
    std::uint64_t num_wins{ 0 };
    std::uint64_t num_not_wins{ 0 };
};

struct PosCombo
{
    Position pos;
    std::uint64_t combo;
};

class DetDice
{
public:
    std::uint32_t roll_dice();
    std::uint32_t roll_dice_thrice();
    DetDice(const std::uint32_t start_val, const std::uint32_t dice_max_val) : dice_val{start_val}, max_val{dice_max_val} {};
private:
    std::uint32_t dice_val{ 1u };
    std::uint32_t max_val{ 100u };
};

std::uint32_t DetDice::roll_dice()
{
    if (dice_val == max_val)
    {
        dice_val = 1u;
        return 100u;
    }
    return dice_val++;
}

std::uint32_t DetDice::roll_dice_thrice()
{
    return roll_dice() + roll_dice() + roll_dice();
}

class QuantumDieController
{
public:
    using Steps = std::uint16_t;
    using Score = std::uint16_t; // When a score >= 21 is reached the corresponding player wins
    using Combos = std::uint64_t; // Tracks how many combinations exist for reaching a specific score

    std::uint64_t play_game();
    ThrowEvalSinglePlayer expand(std::map<Score, std::map<Position,Combos>> &state);
    QuantumDieController(const Position pos_1, const Position pos_2, const Score win) : pos_player_1{ pos_1 }, pos_player_2{ pos_2 }, win_score{ win } {};

private:
    ThrowEvalSinglePlayer expand_single_pos(const Score start_score, const Position start_pos, const Combos start_combo, std::map<Score, std::map<Position,Combos>> &state);
    void fill_start_pos_2_end_pos_map();
    Position advance_track(const Position start, const Steps steps);


    const std::map<Steps, Combos> throw_combo_map{{3,1}, {4,3}, {5,6}, {6,7},
                                                        {7,6}, {8,3}, {9,1}}; ///< maps the steps a player takes after rolling the quantum die thrice to the number of times this step number occurs 
    std::map<Position, std::vector<std::pair<Position, Combos>>> start_pos_2_end_pos_map; ///< Stores for each start position which how many combinations each end position is reachable
    std::uint64_t num_wins_player_1{ 0 };
    std::uint64_t num_wins_player_2{ 0 };
    std::map<Score, std::map<Position,Combos>> state_1;
    std::map<Score, std::map<Position,Combos>> state_2;
    Position pos_player_1;
    Position pos_player_2;
    const Score win_score;
};

ThrowEvalSinglePlayer QuantumDieController::expand(std::map<Score, std::map<Position,Combos>> &state)
{
    ThrowEvalSinglePlayer eval{0,0};
    std::map<Score, std::map<Position,Combos>> state_new;

    for (const auto &score_elem : state)
    {
        const auto &combo_pos_elem = score_elem.second;
        const auto &start_score = score_elem.first;
        for (const auto &combo_pos : combo_pos_elem)
        {
            const auto &start_combos = combo_pos.second;
            const auto &pos = combo_pos.first;
            ThrowEvalSinglePlayer eval_pos = expand_single_pos(start_score, pos, start_combos, state_new);
            eval.num_wins += eval_pos.num_wins;
            eval.num_not_wins += eval_pos.num_not_wins;
        }
    }

    state = state_new;
    return eval;
}


ThrowEvalSinglePlayer QuantumDieController::expand_single_pos(const Score start_score, const Position start_pos, const Combos start_combo, std::map<Score, std::map<Position,Combos>> &state)
{
    ThrowEvalSinglePlayer eval{0,0};
    for (const auto &elem : start_pos_2_end_pos_map.at(start_pos))
    {
        const Position &end_pos = elem.first;
        const Combos &num_combos = elem.second;
        Score new_score = start_score + end_pos;
        Combos new_combos = start_combo * num_combos;
        if (new_score >= win_score)
        {
            eval.num_wins += new_combos;
            continue;
        }
        eval.num_not_wins += new_combos;
        state[new_score][end_pos] = state[new_score][end_pos] + new_combos; // add number of combos to reach this score to already existing combinations
    }
    return eval;
}

std::uint64_t QuantumDieController::play_game()
{
    fill_start_pos_2_end_pos_map();
    state_1[0][pos_player_1] = 1u;
    state_2[0][pos_player_2] = 1u;
    // state_2[0] = std::make_pair(1u, std::set<Position>{pos_player_2});
    ThrowEvalSinglePlayer eval_player_1{ 0u,0u };
    ThrowEvalSinglePlayer eval_player_2{ 0u,0u };

    while(!state_1.empty() || !state_2.empty())
    {
        eval_player_1 = expand(state_1);
        num_wins_player_1 += eval_player_1.num_wins * eval_player_2.num_not_wins;

        if (state_1.empty()) // player 1 definitely wins in all cases after this throw
        {
            break;
        }

        // Execute throw for player 2
        eval_player_2 = expand(state_2);
        num_wins_player_2 += eval_player_2.num_wins*eval_player_1.num_not_wins;

        if (state_2.empty()) // player 2 definitely wins in all cases after this throw
        {
            break;
        }
    }

    return num_wins_player_1 > num_wins_player_2 ? num_wins_player_1 : num_wins_player_2;
}

void QuantumDieController::fill_start_pos_2_end_pos_map()
{
    for (Position start_pos=1u; start_pos<11; ++start_pos)
    {
        for (const auto &die_throw : throw_combo_map)
        {
            Position new_pos = advance_track(start_pos, die_throw.first);
            start_pos_2_end_pos_map[start_pos].push_back(std::make_pair(new_pos, die_throw.second));
        }
    }
}

Position QuantumDieController::advance_track(const Position start, const Steps steps)
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