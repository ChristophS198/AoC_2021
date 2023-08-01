#include <iostream>
#include <vector>
#include <string>

// #include "1/1_sol.cpp"
// #include "2/2_sol.cpp"
// #include "3/3_sol.cpp"
// #include "4/4_sol.cpp"
// #include "5/5_sol.cpp"
// #include "6/6_sol.cpp"
// #include "7/7_sol.cpp"
// #include "8/8_sol.cpp"
// #include "9/9_sol.cpp"
// #include "10/10_sol.cpp"
// #include "11/11_sol.cpp"
// #include "12/12_sol.cpp"
// #include "13/13_sol.cpp"
// #include "14/14_sol.cpp"
// #include "15/15_sol.cpp"
// #include "16/16_sol.cpp"
// #include "17/17_sol.cpp"
// #include "19/19_sol.cpp"
// #include "20/20_sol.cpp"
// #include "21/21_sol.cpp"
// #include "22/22_sol.cpp"
#include "24/24_sol.cpp"

int main(){
    // // Day 1
    // const std::string radar_input_file_path = "1/1_radar_input.txt";

    // std::vector<int> radar_meas = get_measurement(radar_input_file_path);
    // std::cout << count_increases(radar_meas) << std::endl;
    // std::vector<int> acc_radar_meas = sliding_win_measurement(3, radar_meas);
    // std::cout << static_cast<int> (count_increases(acc_radar_meas)) << std::endl;

    // // Day 2 
    // const std::string steering_input_file_path = "2/2_steering_input.txt";

    // const int sol_2 = get_sol_2(steering_input_file_path);
    // std::cout << "End solution: Horizontal Pos x Depth = " << sol_2 << std::endl;


    // // Day 3
    // const std::string diagnostic_report_file_path{"3/3_epsilon_gamma_input.txt"};
    // const unsigned int power_consumption = get_power_consumption(diagnostic_report_file_path);
    // std::cout << "Power consumption = " << power_consumption << std::endl;

    // const unsigned int life_support_rating = get_oxygen_generator_val(diagnostic_report_file_path);
    // std::cout << "Life support rating = " << life_support_rating << std::endl;


    // // Day 4
    // const std::string bingo_input_file_path{"4/4_bingo_input.txt"};
    // std::cout << "Score of last bingo: " << get_last_bingo_score(bingo_input_file_path) << std::endl;;

    // // Day 5
    // const std::string hydrothermal_vents_file{"5/hydrothermal_vents_input.txt"};
    // const unsigned int num_overlaps = get_num_points_larger_x(hydrothermal_vents_file, 1u, 1000u);
    // std::cout << "Number of overlapping grid cells: " << num_overlaps << std::endl;

    // // Day 6
    // const std::string lanternfish_file{"6/lanternfish_population.txt"};
    // unsigned int days = 256u;
    // std::cout << "Number of fish after " << days << " days: " << get_population_size(lanternfish_file, 7u, 9u, days) << std::endl;

    // // Day 7
    // const std::string crab_pos{"7/crab_positions.txt"};    
    // std::cout << "Min fuel: " << calc_min_fuel(crab_pos) << std::endl;

    // // Day 8
    // const std::string signal_entry_list{"8/seven_segment_input.txt"};
    // unsigned int unique_numbers = num_unique_numbers(signal_entry_list);
    // std::cout << "Number of 1,4,7,8 in signal list: " << unique_numbers << std::endl;
    // unsigned int sum_decoded_output = sum_decoded_output_values(signal_entry_list);
    // std::cout << "Sum of decoded output: " << sum_decoded_output << std::endl;

    // // Day 9
    // const std::string height_map{"9/height_map.txt"};
    // std::cout << "Day 9: Sum of risk level: " << get_risk_level_sum(height_map) << std::endl;
    // std::cout << "Day 9: Largest basin product: " << get_largest_basin_product(height_map) << std::endl;

    // // Day 10
    // const std::string navi_subsystem_file{"10/navigation_subsystem.txt"};
    // std::map<char, NaviSubsystemChecker::BracketInfo> bracket_map = {{'(',{')', 3u, 1u}}, 
    //                                                                             {'[',{']', 57u, 2u}},
    //                                                                             {'{',{'}', 1197u, 3u}},
    //                                                                             {'<',{'>', 25137u, 4u}} };
    // NaviSubsystemChecker::ErrorScores err_scores = get_syntax_error_scores(navi_subsystem_file, bracket_map);
    // std::cout << "Day 10: Syntax error score: " << err_scores.syntax_checker_score << std::endl;
    // std::cout << "Day 10: Completion error score: " << err_scores.auto_completion_score << std::endl;

    // // Day 11
    // const std::string dumbo_field{"11/dumbo_octopuses.txt"};
    // std::cout << "Day 11: Flash score after 100 steps: " << get_total_flashes(dumbo_field, 100u) << std::endl;
    // std::cout << "Day 11: Synchronized after step: " << get_synchro_step(dumbo_field, 100u) << std::endl;

    // // Day 12
    // const std::string cavern_connections{"12/cave_connections.txt"};
    // std::cout << "Day 12: Number of paths: " << get_num_paths(cavern_connections) << std::endl;

    // // Day 13
    // const std::string camera_code{"13/camera_code.txt"};
    // std::cout << "Day 13: Number of dots: " << get_num_dots(camera_code) << std::endl;
    
    // // Day 14
    // const std::string polymer_in{"14/polymer_in.txt"};
    // std::cout << "Day 14: Polymer Diff: " << get_element_diff(polymer_in, 40) << std::endl;

    // // Day 15
    // const std::string chitons{"15/chitons_layout.txt"};
    // std::cout << "Day 15: Lowest Risk: " << calc_lowest_risk(chitons) << std::endl;
    // std::cout << "Day 15: Lowest Risk: " << calc_lowest_risk_2(chitons) << std::endl;

    // // Day 16
    // const std::string bits_data{"16/BITS_data.txt"};
    // std::cout << "Accumulated version number: " << acc_version_num(bits_data) << std::endl;
    // std::cout << "Calculated result: " << calc_packet_res(bits_data) << std::endl;

    // //Day 17    
    // std::cout << "Highest trajectory point: " << get_highest_traj_pt("17/target_area.txt") << std::endl;
    // std::cout << "Number of distinct trajectories: " << num_distict_trajectories("17/target_area.txt") << std::endl;

    // //Day 18
    // // implemented in python

    // //Day 19 
    // get_number_of_beacons("19/scanner_data.txt");

    // // Day 20
    // std::cout << "Num lit pixels after 2 iterations: " << day_20_1("20/input_image.txt") << std::endl;
    // std::cout << "Num lit pixels after 50 iterations: " << day_20_2("20/input_image.txt") << std::endl;
    
    // // Day21
    // std::cout << "Score loosing player x die rolls: " << day_21_1("21/starting_pos.txt") << std::endl;
    // std::cout << "Num universes of max wins: " << day_21_2("21/starting_pos.txt") << std::endl;    

    // // Day22
    // std::cout << "22_1: " << day_22_1("22/data.txt") << std::endl;
    // std::cout << "22_2: " << day_22_2("22/data.txt") << std::endl;

    // Day23 was done on paper

    // Day24
    auto res = day_24_1_2("24/data.txt");
    std::cout << "24_1: " << res.second << std::endl;
    std::cout << "24_2: " << res.first << std::endl;


    return 0;
}