#include <string>
#include <vector>
#include <map>
#include <stack>
#include <algorithm>

#include "../utility.h"

class NaviSubsystemChecker
{
public:
    struct BracketInfo
    {
        char counterpart; ///< closing counterpart of a bracket
        unsigned int corrupt_score; ///< score if this bracket corrupts a line
        unsigned int incomplete_score; ///< score if this bracket is missing in a line
    };
    struct ErrorScores
    {
        unsigned long long syntax_checker_score; ///< score of task 1
        unsigned int auto_completion_score; ///< score of task 2
    };

    NaviSubsystemChecker(const std::string& file_path, const std::map<char, BracketInfo>& opening_brackets_map);
    void read_in_navi_data(const std::string& file_path);
    ErrorScores get_syntax_error_scores() const;

private:
    unsigned int check_line_syntax(const std::string& line, std::stack<char>& bracket_stack) const;
    unsigned long long auto_complete_checker(std::stack<char>& bracket_stack) const;
    std::vector<std::string> m_file_content{};
    std::map<char, char> m_op_bracket_map; ///< map to associate opening brackets with their closing counterparts 
    std::map<char, BracketInfo> m_closing_bracket_score; 
};

NaviSubsystemChecker::ErrorScores get_syntax_error_scores(const std::string& file_path, const std::map<char, NaviSubsystemChecker::BracketInfo>& opening_brackets_map)
{
    NaviSubsystemChecker navi_subsystem(file_path, opening_brackets_map);
    return navi_subsystem.get_syntax_error_scores();
}

unsigned int NaviSubsystemChecker::check_line_syntax(const std::string& line, std::stack<char>& bracket_stack) const
{
    bracket_stack = std::stack<char>(); ///< Here we store the closing brackets in the order we expect them

    for (const auto& c : line)
    {
        if (m_closing_bracket_score.count(c) == 1) // If it is a closing bracket
        {
            if (bracket_stack.size() == 0)
            {
                return m_closing_bracket_score.at(c).corrupt_score;
                // throw std::runtime_error("Received closing bracket, although stack is empty!");
            }
            if (bracket_stack.top() == c)
            {
                bracket_stack.pop();
            }
            else
            {
                bracket_stack = std::stack<char>(); ///< emtpy stack before returning
                return m_closing_bracket_score.at(c).corrupt_score;
            }
        }
        else
        {
            if (m_op_bracket_map.count(c) == 1)
            {
                bracket_stack.push(m_op_bracket_map.at(c));
            }
            else
            {
                std::string err_msg = std::string("Received unexpected sign '") + c + std::string("' during parsing of line: \n") + line;
                throw std::runtime_error(err_msg);
            }
        }
    }
    return 0u;
}


NaviSubsystemChecker::NaviSubsystemChecker( const std::string& file_path, 
                                            const std::map<char, BracketInfo>& opening_brackets_map) 
{
    for (const auto& bracket : opening_brackets_map)
    {
        m_closing_bracket_score[bracket.second.counterpart] = {bracket.first, bracket.second.corrupt_score, bracket.second.incomplete_score};
        m_op_bracket_map[bracket.first] = bracket.second.counterpart;
    }
    read_in_navi_data(file_path);
}

void NaviSubsystemChecker::read_in_navi_data(const std::string& file_path)
{
    m_file_content = read_string_vec_from_file(file_path);
}

unsigned long long NaviSubsystemChecker::auto_complete_checker(std::stack<char>& bracket_stack) const
{
    unsigned long long score{0ull};
    while (bracket_stack.size() > 0)
    {
        score = 5*score + m_closing_bracket_score.at(bracket_stack.top()).incomplete_score;
        bracket_stack.pop();
    }
    return score;
}


NaviSubsystemChecker::ErrorScores NaviSubsystemChecker::get_syntax_error_scores() const
{
    NaviSubsystemChecker::ErrorScores error_scores{0,0};
    std::vector<unsigned long long> completion_err_per_line{};
    for (const auto& line : m_file_content)
    {
        std::stack<char> bracket_stack{};
        error_scores.syntax_checker_score += static_cast<unsigned long long>(check_line_syntax(line, bracket_stack));
        if (bracket_stack.size() > 0)
        {
            completion_err_per_line.push_back(auto_complete_checker(bracket_stack));
        }
    }
    std::sort(completion_err_per_line.begin(), completion_err_per_line.end());
    if (completion_err_per_line.size() % 2 == 0)
    {
        throw std::runtime_error("Number of incomplete lines (lines that are not corrupted) in navigation subsystem input file must be an odd number!");
    }
    error_scores.auto_completion_score = completion_err_per_line[completion_err_per_line.size() / 2];
    return error_scores;
}
