#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <fstream>

#include "../utility.h"

using coord = int;

struct Dot
{ 
    Dot(coord x, coord y) : m_x{x}, m_y{y} {};
    void fold_up(const coord fold_y);
    void fold_left(const coord fold_x);

    coord m_x{};
    coord m_y{};
};

void Dot::fold_up(const coord fold_y)
{
    m_y = m_y <= fold_y ? m_y : fold_y - (m_y - fold_y); 
}

void Dot::fold_left(const coord fold_x)
{
    m_x = m_x <= fold_x ? m_x : fold_x - (m_x - fold_x); 
}

bool operator<(const Dot& dot1, const Dot& dot2)
{
    return (dot1.m_y < dot2.m_y || ((dot1.m_y == dot2.m_y) && dot1.m_x < dot2.m_x));
}

bool operator==(const Dot& dot1, const Dot& dot2)
{
    return dot1.m_x == dot2.m_x && dot1.m_y == dot2.m_y;
}

struct FoldOperation
{
    enum class OperationType
    {
        FOLD_UP,
        FOLD_LEFT
    };
    FoldOperation(const OperationType type, const coord fold_line) : m_op_type{type}, m_fold_line{fold_line} {};
    OperationType m_op_type;
    coord m_fold_line; 
};


class TransparencyDecoder
{
public:
    TransparencyDecoder(const std::string& file_path);
    void read_in_dots_and_folds(const std::string& file_path);
    size_t get_num_dots() const;
    void execute_folds();

    std::ostream& print_dots(std::ostream& os);

private: 
    void remove_duplicates();
    std::vector<Dot> m_dots{};
    std::vector<FoldOperation> m_fold_coord_vec{};
    size_t m_dim_x{0};
    size_t m_dim_y{0};
};

TransparencyDecoder::TransparencyDecoder(const std::string& file_path)
{
    read_in_dots_and_folds(file_path);
}

void TransparencyDecoder::execute_folds()
{
    for (const FoldOperation op : m_fold_coord_vec)
    {
        if (op.m_op_type == FoldOperation::OperationType::FOLD_LEFT)
        {
            for (Dot& dot : m_dots)
            {
                dot.fold_left(op.m_fold_line);
            }
        }
        else if (op.m_op_type == FoldOperation::OperationType::FOLD_UP)
        {
            for (Dot& dot : m_dots)
            {
                dot.fold_up(op.m_fold_line);
            }
        }
        remove_duplicates();
        if (op.m_op_type == FoldOperation::OperationType::FOLD_LEFT)
        {
            m_dim_x = op.m_fold_line - 1;
        }
        else if (op.m_op_type == FoldOperation::OperationType::FOLD_UP)
        {
            m_dim_y = op.m_fold_line - 1;
        }
    }
}

void TransparencyDecoder::remove_duplicates()
{
    std::sort(m_dots.begin(), m_dots.end()); 
    auto last = std::unique(m_dots.begin(), m_dots.end());
    m_dots.erase(last, m_dots.end());
}

void TransparencyDecoder::read_in_dots_and_folds(const std::string& file_path)
{
    std::vector<std::string> file_input = read_string_vec_from_file(file_path);
    size_t i{0};
    // Read coordinates of dots
    for (; i<file_input.size(); ++i)
    {
        if (file_input[i].empty()) 
        {
            break;
        }
        std::vector<int> coord_vec = parse_string_to_number_vec<int>(file_input[i]);
        if (coord_vec.size() != 2) 
        {
            throw std::runtime_error("Size coords must be equal to 2!");
        }
        m_dim_x = m_dim_x < static_cast<size_t>(coord_vec[0]) ? static_cast<size_t>(coord_vec[0]) : m_dim_x;
        m_dim_y = m_dim_y < static_cast<size_t>(coord_vec[1]) ? static_cast<size_t>(coord_vec[1]) : m_dim_y;
        m_dots.push_back(Dot{coord_vec[0], coord_vec[1]});
    }
    ++i;

    // Read fold operations
    for (; i<file_input.size(); ++i)
    {
        std::string line = file_input[i];
        if (!line.empty())
        {
            int fold_line = get_number_after_str<int>(line, "=");
            if (line[11] == 'x')
            {
                m_fold_coord_vec.push_back(FoldOperation{FoldOperation::OperationType::FOLD_LEFT, fold_line});
            }
            else if (line[11] == 'y')
            {
                m_fold_coord_vec.push_back(FoldOperation{FoldOperation::OperationType::FOLD_UP, fold_line});
            }
        }
    }
}

size_t TransparencyDecoder::get_num_dots() const
{
    return m_dots.size();
}

std::ostream& TransparencyDecoder::print_dots(std::ostream& os)
{
    size_t r{0};
    size_t c{0};
    size_t i{0};
    remove_duplicates(); // does also sort all elements as need below
    os << "\n";
    while (r <= m_dim_y)
    {
        while (c <= m_dim_x)
        {
            if (i < m_dots.size() && r == static_cast<size_t>(m_dots[i].m_y) 
                && c == static_cast<size_t>(m_dots[i].m_x)) 
            {
                os << "#";
                ++i;
            }
            else
            {
                os << ".";
            }
            ++c;
        }
        os << "\n";
        ++r;
        c = 0;
    }
    os << "\n";
    return os;
}