#include <string>
#include <memory>
#include <cassert>

#include "../utility.h"

enum class LenTypeId : std::uint8_t
{
    LEN_IN_BITS,
    NUM_SUB_PACKETS
};

enum class PackTypeId : std::uint8_t
{
    SUM, 
    PRODUCT, 
    MIN, 
    MAX, 
    LITERAL, 
    GT, 
    LT, 
    EQ
};

using cType = std::uint64_t;

struct Packet
{
    std::uint8_t version{};
    PackTypeId type_id{};
    std::vector<std::unique_ptr<Packet>> sub_packets{};
    Packet() = default;
    Packet(std::uint8_t ver, PackTypeId type) : version{ver}, type_id{type} {};
};

struct OperatorPackage : public Packet
{
    std::uint32_t num_sub_packets{0u};
    OperatorPackage() = default;
    OperatorPackage(std::uint8_t ver, PackTypeId type, std::uint32_t num) : Packet(ver, type), num_sub_packets{num} {};
};

struct LiteralPackage : public Packet
{
    cType dec_number{};
    LiteralPackage() = default;
    LiteralPackage(std::uint8_t ver, PackTypeId type, cType num) : Packet(ver, type), dec_number{num} {};
};

class BitsParser
{
public: 

    std::unique_ptr<Packet> parse_data(const std::string &file_path) const;
    std::uint32_t acc_versions(const std::unique_ptr<Packet> &packet) const;
    cType calc_rec(const std::unique_ptr<Packet> &packet);

private:
    std::string read_hex_from_file(const std::string &file_path) const;
    std::string convert_to_binary(const std::string hex_in) const;
    std::unique_ptr<LiteralPackage> decode_literal_package(const std::string &pack, const size_t pos, size_t &new_pos) const;
    std::unique_ptr<OperatorPackage> decode_operator_package(const std::string &pack, const size_t pos, size_t &new_pos) const;
    std::unique_ptr<Packet> decode_packet(const std::string &packet, const size_t pos) const;
    PackTypeId get_type(const std::string &pack, const size_t pos) const;
};

cType BitsParser::calc_rec(const std::unique_ptr<Packet> &packet)
{
    const OperatorPackage *op_ptr = nullptr;
    if (packet->type_id != PackTypeId::LITERAL)
    {
        op_ptr = static_cast<const OperatorPackage*>(packet.get());
        if (op_ptr->num_sub_packets == 0)
        {
            return 0u;
        }
    }

    switch(packet->type_id)
    {
        case PackTypeId::SUM: 
        {
            cType sum{0};
            for (size_t i=0; i<op_ptr->num_sub_packets; ++i)
            {
                sum += calc_rec(op_ptr->sub_packets[i]);
            }
            return sum;
            break;
        }
        case PackTypeId::PRODUCT: 
        {
            cType prod{1};
            for (size_t i=0; i<op_ptr->num_sub_packets; ++i)
            {
                prod *= calc_rec(op_ptr->sub_packets[i]);
            }
            return prod;
            break;
        }
        case PackTypeId::MIN: 
        {
            cType min_val{calc_rec(op_ptr->sub_packets.at(0))};
            for (size_t i=1; i<op_ptr->num_sub_packets; ++i)
            {
                cType tmp = calc_rec(op_ptr->sub_packets.at(i));
                min_val = min_val > tmp ? tmp : min_val;
            }
            return min_val;
            break;
        }
        case PackTypeId::MAX: 
        {
            cType max_val{calc_rec(op_ptr->sub_packets.at(0))};
            for (size_t i=1; i<op_ptr->num_sub_packets; ++i)
            {
                cType tmp = calc_rec(op_ptr->sub_packets.at(i));
                max_val = max_val < tmp ? tmp : max_val;
            }
            return max_val;
            break;
        }
        case PackTypeId::LITERAL: 
            return static_cast<const LiteralPackage*>(packet.get())->dec_number;
            break;
        case PackTypeId::GT: 
            assert(op_ptr->num_sub_packets == 2u);
            if (calc_rec(op_ptr->sub_packets[0]) > calc_rec(op_ptr->sub_packets[1]))
            {
                return 1ull;
            }
            break;
        case PackTypeId::LT: 
            assert(op_ptr->num_sub_packets == 2u);
            if (calc_rec(op_ptr->sub_packets[0]) < calc_rec(op_ptr->sub_packets[1]))
            {
                return 1ull;
            }
            break;
        case PackTypeId::EQ: 
            assert(op_ptr->num_sub_packets == 2u);
            if (calc_rec(op_ptr->sub_packets[0]) == calc_rec(op_ptr->sub_packets[1]))
            {
                return 1ull;
            }
            break;
        default:
            throw std::runtime_error("Unknown operator type received!");
    }
    return 0ull;
}

std::unique_ptr<Packet> BitsParser::decode_packet(const std::string &packet, const size_t pos) const
{
    assert(packet.length() > 5u);
    PackTypeId type_id = get_type(packet, pos);
    size_t new_pos{0};
    if (type_id == PackTypeId::LITERAL)
    {
        return decode_literal_package(packet, pos, new_pos);
    }
    else
    {
        return decode_operator_package(packet, pos, new_pos);
    }
}

std::unique_ptr<OperatorPackage> BitsParser::decode_operator_package(const std::string &packet, const size_t pos, size_t &new_pos) const
{
    assert(packet.length() > 5u);
    std::unique_ptr<OperatorPackage> op_packet = std::make_unique<OperatorPackage>();
    op_packet->version = static_cast<std::uint8_t>(bin_str_to_uint(packet.substr(pos,3)));
    op_packet->type_id = get_type(packet, pos);
    size_t cur_pos{pos+6};

    // distinguish between length specification in bits or number of packets
    LenTypeId len_type_id = packet.at(cur_pos++) == '0' ? LenTypeId::LEN_IN_BITS : LenTypeId::NUM_SUB_PACKETS;
    size_t relevant_bits = len_type_id == LenTypeId::LEN_IN_BITS ? 15u : 11u;

    // unpack packet
    std::uint32_t len_decl = static_cast<std::uint32_t>(bin_str_to_uint(packet.substr(cur_pos, relevant_bits)));
    cur_pos += relevant_bits;
    size_t end_pos = cur_pos + len_decl;
    std::uint32_t num_sub_packets{0};
    while ((LenTypeId::LEN_IN_BITS == len_type_id && cur_pos < end_pos)
            || (LenTypeId::NUM_SUB_PACKETS == len_type_id && num_sub_packets < len_decl))
    {
        ++num_sub_packets;
        size_t tmp_pos{};
        if (PackTypeId::LITERAL == get_type(packet, cur_pos))
        { // Literal sub-packet
            op_packet->sub_packets.push_back(decode_literal_package(packet, cur_pos, tmp_pos));
        }
        else // Operator sub-packet
        {
            op_packet->sub_packets.push_back(decode_operator_package(packet, cur_pos, tmp_pos));
        }
        cur_pos = tmp_pos;
    }
    op_packet->num_sub_packets = num_sub_packets;
    new_pos = cur_pos;
    return op_packet;
}

std::unique_ptr<LiteralPackage> BitsParser::decode_literal_package(const std::string &pack, const size_t pos, size_t &new_pos) const
{
    assert(pack.length() > 5u);
    std::uint8_t version = static_cast<std::uint8_t>(bin_str_to_uint(pack.substr(pos,3)));
    PackTypeId type_id = get_type(pack, pos);
    size_t cur_pos{pos+6};
    std::string bin_str;
    bool end_reached{false};
    while (!end_reached)
    {
        bin_str += pack.substr(cur_pos+1,4);
        end_reached = pack.at(cur_pos) == '0';
        cur_pos += 5;
    }
    // size_t num_pads = (4 - (cur_pos - pos) % 4) % 4; 
    // new_pos = cur_pos + num_pads;
    new_pos = cur_pos;
    cType dec_number = bin_str_to_uint(bin_str);

    return std::make_unique<LiteralPackage>(version, type_id, dec_number);
}

std::string BitsParser::convert_to_binary(const std::string hex_in) const
{
    std::string binary;
    for (const auto c : hex_in)
    {
        switch(c) 
        {
            case '0': 
                binary += "0000";
                break;
            case '1': 
                binary += "0001";
                break;
            case '2': 
                binary += "0010";
                break;
            case '3': 
                binary += "0011";
                break;
            case '4': 
                binary += "0100";
                break;
            case '5': 
                binary += "0101";
                break;
            case '6': 
                binary += "0110";
                break;
            case '7': 
                binary += "0111";
                break;
            case '8': 
                binary += "1000";
                break;
            case '9': 
                binary += "1001";
                break;
            case 'A': 
                binary += "1010";
                break;
            case 'B': 
                binary += "1011";
                break;
            case 'C': 
                binary += "1100";
                break;
            case 'D': 
                binary += "1101";
                break;
            case 'E': 
                binary += "1110";
                break;
            case 'F': 
                binary += "1111";
                break;
            default: 
                throw std::runtime_error("Unexpected char in input BITS found!");
        }
    }
    return binary;
}


std::string BitsParser::read_hex_from_file(const std::string& file_path) const
{
    std::fstream input_file;
    input_file.open(file_path,std::ios::in);
    std::string input_line;
    if (input_file.is_open()){
        getline(input_file, input_line);  //read data from file object and put it into string.
        input_file.close();   //close the file object.
    }
    return input_line;
}


std::unique_ptr<Packet> BitsParser::parse_data(const std::string &file_path) const
{
    std::string hex_data = read_hex_from_file(file_path);
    std::string bin_data = convert_to_binary(hex_data);
    return decode_packet(bin_data, 0u);
}


PackTypeId BitsParser::get_type(const std::string &pack, const size_t pos) const
{
    assert(pack.length() > 5u);
    return static_cast<PackTypeId>(bin_str_to_uint(pack.substr(pos+3,3)));
}

std::uint32_t BitsParser::acc_versions(const std::unique_ptr<Packet> &packet) const
{
    std::uint32_t versions{0};
    versions += static_cast<std::uint32_t>(packet->version);

    for (size_t i=0; i < packet->sub_packets.size(); ++i)
    {
        versions += acc_versions(packet->sub_packets[i]);
    }
    return versions;
}
