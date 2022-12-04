#include <string>

#include "bits_parser.h"

std::uint16_t acc_version_num(const std::string &file_path)
{
    BitsParser bit_parser{};

    std::unique_ptr<Packet> root_packet = bit_parser.parse_data(file_path);
    return bit_parser.acc_versions(root_packet);
}

std::uint64_t calc_packet_res(const std::string &file_path)
{
    BitsParser bit_parser{};

    std::unique_ptr<Packet> root_packet = bit_parser.parse_data(file_path);
    return bit_parser.calc_rec(root_packet);
}