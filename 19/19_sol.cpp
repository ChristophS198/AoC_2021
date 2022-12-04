#include <string>
#include <map>
#include <vector>
#include <set>
#include <iomanip>
#include <utility>

#include "../utility.h"

namespace {
constexpr float SIMILARITY_THRES = 1.0f;
const std::string TARGET_SYSTEM{ "scanner 0" };

using tCoord = int;

struct Trafo
{
    int xx;
    int xy;
    int xz;
    int yx;
    int yy;
    int yz;
    int zx;
    int zy;
    int zz;
    int tx;
    int ty;
    int tz;
};

std::ostream& operator<<(std::ostream &out, const Trafo &trafo)
{
    constexpr std::uint16_t w = 6;
    out << std::setw(w) << trafo.xx << " " << std::setw(w) << trafo.xy << " " << std::setw(w) << trafo.xz << std::setw(w) << trafo.tx << "\n"; 
    out << std::setw(w) << trafo.yx << " " << std::setw(w) << trafo.yy << " " << std::setw(w) << trafo.yz << std::setw(w) << trafo.ty << "\n"; 
    out << std::setw(w) << trafo.zx << " " << std::setw(w) << trafo.zy << " " << std::setw(w) << trafo.zz << std::setw(w) << trafo.tz << "\n"; 
    return out;
}

Trafo operator*(const Trafo &t1, const Trafo &t2)
{
    Trafo res{};
    res.xx = t1.xx*t2.xx + t1.xy*t2.yx + t1.xz*t2.zx;
    res.xy = t1.xx*t2.xy + t1.xy*t2.yy + t1.xz*t2.zy;
    res.xz = t1.xx*t2.xz + t1.xy*t2.yz + t1.xz*t2.zz;
    res.yx = t1.yx*t2.xx + t1.yy*t2.yx + t1.yz*t2.zx;
    res.yy = t1.yx*t2.xy + t1.yy*t2.yy + t1.yz*t2.zy;
    res.yz = t1.yx*t2.xz + t1.yy*t2.yz + t1.yz*t2.zz;
    res.zx = t1.zx*t2.xx + t1.zy*t2.yx + t1.zz*t2.zx;
    res.zy = t1.zx*t2.xy + t1.zy*t2.yy + t1.zz*t2.zy;
    res.zz = t1.zx*t2.xz + t1.zy*t2.yz + t1.zz*t2.zz;
    res.tx = t1.xx*t2.tx + t1.xy*t2.ty + t1.xz*t2.tz + t1.tx;
    res.ty = t1.yx*t2.tx + t1.yy*t2.ty + t1.yz*t2.tz + t1.ty;
    res.tz = t1.zx*t2.tx + t1.zy*t2.ty + t1.zz*t2.tz + t1.tz;
    return res;
}

std::map<std::string, std::vector<Point3D<tCoord>>> read_scanner_report(const std::string &file_path);
template<typename T>
std::map<Point3D<tCoord>,std::vector<T>> calc_connection_ids(const std::vector<Point3D<tCoord>> &beacon_vec);
template<typename T>
std::vector<std::pair<Point3D<tCoord>, Point3D<tCoord>>> get_overlapping_beacons(const std::map<Point3D<tCoord>, std::vector<T>> &point_map_1, const std::map<Point3D<tCoord>, std::vector<T>>&point_map_2);
std::vector<Point3D<tCoord>> transform_points(const Trafo m, const std::vector<Point3D<tCoord>> &point_map);
Point3D<tCoord> transform_point(const Trafo m, const Point3D<tCoord> &point);
Trafo get_trafo(const std::vector<std::pair<Point3D<tCoord>, Point3D<tCoord>>> &overlap_pts);
bool is_unique_abs_elem(const tCoord val, const Point3D<tCoord> pt);
int get_trafo_elem(const tCoord c1, const tCoord c2);
std::map<std::string, std::vector<std::string>> get_transformation_chain(const std::map<std::string, std::vector<std::string>> &r_t_assignment, const std::string target_system);
std::map<std::string, std::vector<Point3D<tCoord>>> transform_pts(const std::map<std::string, std::vector<Point3D<tCoord>>> &scan_map, const std::map<std::string, std::vector<std::string>> &trafo_map, const std::map<std::string, std::map<std::string,Trafo>> &report_trafo_assignment, const std::string target_system);
std::set<Point3D<tCoord>> get_unique_pts(const std::map<std::string, std::vector<Point3D<tCoord>>> &scan_map);
std::uint32_t get_largest_dist(const std::map<std::string, std::vector<std::string>> &trafo_map, const std::map<std::string, std::map<std::string,Trafo>> &report_trafo_assignment);
std::uint32_t manhatten_dist(const Point3D<tCoord> &p1, const Point3D<tCoord> &p2);


void print_scanner_map(std::map<std::string, std::vector<Point3D<tCoord>>> scan_map)
{
        for (const auto &scanner : scan_map)
    {
        std::cout << scanner.first << std::endl;
        for (const auto &point : scanner.second)
        {
            std::cout << point.x << ", " << point.y << ", " << point.z << std::endl;
        }
        std::cout << std::endl;
    }
    std::cout << "Scanner size: " << scan_map.size() << std::endl;
}

void print_similar_points(const std::vector<std::pair<Point3D<tCoord>, Point3D<tCoord>>> &similar_pt_ids, const std::string &name1, const std::string &name2)
{
    std::cout << "Similar connections between " << name1 << " and " << name2 << std::endl;
    for (const auto &ind_pair : similar_pt_ids)
    {
        std::cout << ind_pair.first << " - " << ind_pair.second << "\n";
    }
    std::cout << std::endl;
}

template<typename T>
void print_id_map(std::ostream &out, const std::map<Point3D<tCoord>, std::vector<std::int32_t>> &id_map, const std::string &name)
{
    out << "\n" << name << "\n";
    out << std::setw(14) << " ";
    for (size_t col_c=0; col_c < id_map.size(); ++col_c)
    {
        out << std::setw(8) << col_c << "  "; 
    }
    out << "\n";

    size_t row_c{0};
    for (const auto elem : id_map)
    {
        // out << std::fixed << std::setw(6) << row_c++ << ": ";
        out << std::fixed << std::setw(4) << elem.first.x << "," << std::setw(4) << elem.first.y << "," << std::setw(4) << elem.first.z << ": ";
        for (const auto weight : elem.second)
        {
            out << std::fixed << std::setw(8) << weight << ", ";
            // out << std::fixed << std::setw(6) << elem << ", ";
        }
        out << "\n";
    }
}

void print_trafo(Trafo trafo, const std::string &name1, const std::string &name2)
{
    std::cout << "Trafo between " << name1 << " and " << name2 << std::endl;
    std::cout << std::setw(2) << trafo.xx << " " << std::setw(2) << trafo.xy << " " << std::setw(2) << trafo.xz << std::setw(6) << trafo.tx << "\n"; 
    std::cout << std::setw(2) << trafo.yx << " " << std::setw(2) << trafo.yy << " " << std::setw(2) << trafo.yz << std::setw(6) << trafo.ty << "\n"; 
    std::cout << std::setw(2) << trafo.zx << " " << std::setw(2) << trafo.zy << " " << std::setw(2) << trafo.zz << std::setw(6) << trafo.tz << "\n"; 
}

std::uint16_t get_number_of_beacons(const std::string &file_path)
{
    std::map<std::string, std::vector<Point3D<tCoord>>> scan_map = read_scanner_report(file_path);
    std::map<std::string, std::map<Point3D<tCoord>, std::vector<std::int32_t>>> scanner_point_map{};
    std::ofstream my_file("IDs.txt");
    // iterate through each scanner report and calc for each beacon a vector of connection weights
    for (const auto &mat : scan_map)
    {
        scanner_point_map[mat.first] = calc_connection_ids<std::int32_t>(mat.second);
        // print_id_map<std::int32_t>(my_file, scanner_point_map[mat.first], mat.first);
        my_file << "\n";
    }  

    std::map<std::string, std::vector<std::string>> report_report_assignment; // stores for each report all reports that share at least 12 points -> trafo exists
    std::map<std::string, std::map<std::string,Trafo>> report_trafo_assignment;
    // compare each point of a scanner report to each point of the remaining reports
    for (const auto &mat : scan_map)
    {
        std::vector<std::string> trafo_vec;
        std::map<std::string, Trafo> trafo_map;
        for (const auto &mat_2 : scan_map)
        {
            if (mat.first == mat_2.first) 
            {
                continue;
            }
            else
            {

                std::vector<std::pair<Point3D<tCoord>, Point3D<tCoord>>> similar_pt_ids = get_overlapping_beacons<std::int32_t>(scanner_point_map[mat.first],scanner_point_map[mat_2.first]);
                // print_similar_points(similar_pt_ids, mat.first, mat_2.first);
                if (similar_pt_ids.size() >= 11)
                {
                    Trafo trafo = get_trafo(similar_pt_ids);
                    trafo_map[mat_2.first] = trafo;
                    trafo_vec.push_back(mat_2.first);
                    // print_trafo(trafo, mat.first, mat_2.first);
                    std::vector<Point3D<tCoord>> trafo_pt_vec;
                }
            }
        }
        report_report_assignment[mat.first] = trafo_vec;
        report_trafo_assignment[mat.first] = trafo_map;
    }

    std::map<std::string, std::vector<std::string>> trafo_map; // Stores for each scanner report a vector of transformations to be executed to transform points to system of report 0
    trafo_map = get_transformation_chain(report_report_assignment, TARGET_SYSTEM);

    std::map<std::string, std::vector<Point3D<tCoord>>> scan_map_transformed = transform_pts(scan_map, trafo_map, report_trafo_assignment, TARGET_SYSTEM);
    // std::cout << std::endl;
    // print_scanner_map(scan_map_transformed);
    std::set<Point3D<tCoord>> unique_pts = get_unique_pts(scan_map_transformed);
    std::cout << "Largest Manhatten Dist: " << get_largest_dist(trafo_map, report_trafo_assignment) << std::endl;
    std::cout << "Number of beacons: " << unique_pts.size() << std::endl;
    return unique_pts.size();
}

std::uint32_t get_largest_dist(const std::map<std::string, std::vector<std::string>> &trafo_map, const std::map<std::string, std::map<std::string,Trafo>> &report_trafo_assignment)
{
    std::vector<std::pair<std::string,Point3D<tCoord>>> translation_vec;
    for (const auto &rep_trafo_vec : trafo_map)
    {
        if (rep_trafo_vec.second.size() == 1) // vector for target system has size of 1
        {
            continue;
        }
        std::string start_system = rep_trafo_vec.first;
        const std::vector<std::string> &trafos = rep_trafo_vec.second;
        Trafo overall_trafo = report_trafo_assignment.at(trafos.at(0)).at(trafos.at(1));
        for (size_t i=1; i<trafos.size()-1; ++i)
        {
             overall_trafo = overall_trafo * report_trafo_assignment.at(trafos.at(i)).at(trafos.at(i+1));
        }
        // std::cout << overall_trafo << "\n" <<std::endl;
        translation_vec.push_back(std::make_pair(start_system, Point3D<tCoord>{overall_trafo.tx, overall_trafo.ty, overall_trafo.tz}));
    }

    translation_vec.push_back(std::make_pair(TARGET_SYSTEM, Point3D<tCoord>{0,0,0}));

    std::uint32_t largest_dist{0};
    for (size_t i=0; i<translation_vec.size(); ++i)
    {
        for (size_t j=i+1; j<translation_vec.size(); ++j)
        {
            std::uint32_t dist = manhatten_dist(translation_vec[i].second, translation_vec[j].second);
            if (dist > largest_dist)
            {
                largest_dist = dist;
                std::uint32_t dist2 = manhatten_dist(translation_vec[i].second, translation_vec[j].second);
            }
        }
    }
    return largest_dist;
}

std::uint32_t manhatten_dist(const Point3D<tCoord> &p1, const Point3D<tCoord> &p2)
{
    int tmp = std::abs(p2.x - p1.x) + std::abs(p2.y - p1.y) + std::abs(p2.z - p1.z);
    return static_cast<std::uint32_t>(tmp);
}

std::set<Point3D<tCoord>> get_unique_pts(const std::map<std::string, std::vector<Point3D<tCoord>>> &scan_map)
{
    std::set<Point3D<tCoord>> pt_set;
    for (const auto &report : scan_map)
    {
        for (const auto &pt : report.second)
        {
            pt_set.insert(pt);
        }
    }
    return pt_set;
}

std::vector<Point3D<tCoord>> transform_points(const Trafo m, const std::vector<Point3D<tCoord>> &point_vec)
{
    std::vector<Point3D<tCoord>> trafo_pt_vec;
    for (const auto &pt : point_vec)
    {
        trafo_pt_vec.push_back(transform_point(m, pt));
    }
    return trafo_pt_vec;
}


Point3D<tCoord> transform_point(const Trafo m, const Point3D<tCoord> &point)
{
    Point3D<tCoord> transformed{};
    transformed.x = m.xx * point.x + m.xy * point.y + m.xz * point.z + m.tx;
    transformed.y = m.yx * point.x + m.yy * point.y + m.yz * point.z + m.ty;
    transformed.z = m.zx * point.x + m.zy * point.y + m.zz * point.z + m.tz;
    return transformed;
}


Trafo get_trafo(const std::vector<std::pair<Point3D<tCoord>, Point3D<tCoord>>> &overlap_pts)
{
    if (overlap_pts.size() < 12)
    {
        return {};
    }
    // Calc rotation between two scanner reports
    bool trafo_x_found{false};
    bool trafo_y_found{false};
    bool trafo_z_found{false};
    Trafo trafo{};
    size_t ind{0};
    while (!(trafo_x_found && trafo_y_found && trafo_z_found))
    {
        if (ind == overlap_pts.size())
        {
            throw std::runtime_error("Unable to find transformation matrix!");
        }
        const auto vec_1 = overlap_pts[ind].first - overlap_pts[ind+1].first;
        const auto vec_2 = overlap_pts[ind].second - overlap_pts[ind+1].second;
        if (is_unique_abs_elem(vec_1.x, vec_1))
        {
            trafo.xx = get_trafo_elem(vec_1.x, vec_2.x);
            trafo.xy = get_trafo_elem(vec_1.x, vec_2.y);
            trafo.xz = get_trafo_elem(vec_1.x, vec_2.z);
            trafo_x_found = true;
        }
        if (is_unique_abs_elem(vec_1.y, vec_1))
        {
            trafo.yx = get_trafo_elem(vec_1.y, vec_2.x);
            trafo.yy = get_trafo_elem(vec_1.y, vec_2.y);
            trafo.yz = get_trafo_elem(vec_1.y, vec_2.z);
            trafo_y_found = true;
        }
        if (is_unique_abs_elem(vec_1.z, vec_1))
        {
            trafo.zx = get_trafo_elem(vec_1.z, vec_2.x);
            trafo.zy = get_trafo_elem(vec_1.z, vec_2.y);
            trafo.zz = get_trafo_elem(vec_1.z, vec_2.z);
            trafo_z_found = true;
        }        
        ++ind;
    }

    // Find translation
    trafo.tx = 0;
    trafo.ty = 0;
    trafo.tz = 0;

    Point3D<tCoord> translation = overlap_pts[0].first - transform_point(trafo, overlap_pts[0].second);
    trafo.tx = translation.x;
    trafo.ty = translation.y;
    trafo.tz = translation.z;
    return trafo;
}

int get_trafo_elem(const tCoord c1, const tCoord c2)
{
    if (c1 == c2)
    {
        return 1;
    }
    else
    {
        if (c1 == -c2)
        {
            return -1;
        }
        else 
        {
            return 0;
        }
    }
}

bool is_unique_abs_elem(const tCoord val, const Point3D<tCoord> pt)
{
    std::uint8_t count{0};
    if (std::abs(pt.x) == std::abs(val)) ++count;
    if (std::abs(pt.y) == std::abs(val)) ++count;
    if (std::abs(pt.z) == std::abs(val)) ++count;
    
    if (count == 1) return true;
    else            return false;
}

// Read in data
std::map<std::string, std::vector<Point3D<tCoord>>> read_scanner_report(const std::string &file_path)
{
    std::map<std::string, std::vector<Point3D<tCoord>>> scanner_map{};
    std::fstream input_file;
    input_file.open(file_path,std::ios::in);
    if (input_file.is_open()){
        std::string input_line{};
        std::string cur_scanner{};
        while(getline(input_file, input_line)){  //read data from file object and put it into string.
            if (input_line.substr(0, 3) == "---")
            {
                cur_scanner = input_line.substr(4, input_line.length()-8);
                scanner_map[cur_scanner] = std::vector<Point3D<tCoord>>{};
            }
            else
            {
                if (input_line != "")
                {
                    std::vector<tCoord> num_vec = parse_string_to_number_vec<tCoord>(input_line);
                    if (num_vec.size() != 3)
                    {
                        throw std::runtime_error("More/less than 3 numbers were read from scanner report!");
                    }
                    scanner_map[cur_scanner].push_back(Point3D<tCoord>{num_vec[0], num_vec[1], num_vec[2]});
                }
            }
        }
        input_file.close();   //close the file object.
    }
    return scanner_map;
}

/**
 * @brief calculates for each beacon a vector of values 
 * Those values should be indifferent to rotation and as distinct as possible so if the same beacon 
 * is found in another report its entries are similar
 * 
 * @param beacons vector of Point3D beacons
 * @return std::map<size_t,std::vector<T>>
 */
template<typename T>
std::map<Point3D<tCoord>,std::vector<T>> calc_connection_ids(const std::vector<Point3D<tCoord>> &beacon_vec)
{
    std::map<Point3D<tCoord>, std::vector<T>> point_weight_map{};
    for (size_t point_id=0; point_id < beacon_vec.size(); ++point_id)
    {
        std::vector<T> id_vec{};
        for (size_t elem=0; elem < beacon_vec.size(); ++elem)
        {
            id_vec.push_back(static_cast<T>(dist_squ<tCoord>(Point3D<T>{beacon_vec[elem] - beacon_vec[point_id]})));
        }
        point_weight_map[beacon_vec[point_id]] = id_vec;
    }
    return point_weight_map;
}

// Compare points between two scanner reports and select candidates which have more than 12 
template<typename T>
std::vector<std::pair<Point3D<tCoord>, Point3D<tCoord>>> get_overlapping_beacons(const std::map<Point3D<tCoord>, std::vector<T>> &point_map_1, const std::map<Point3D<tCoord>, std::vector<T>>&point_map_2)
{
    std::vector<std::pair<Point3D<tCoord>, Point3D<tCoord>>> similar_pts{};
    // Für jeden Punkt gibt es in point_map_1 und _2 eine Matrix
    // Wenn ein Vergleich der beiden Matrizen genug gleiche Elemente ergibt -> Punkt merken 
    // Diagonaleinträge sollten wohl außen vor gelassen werden, genauso wie jedes Element doppelt drin ist wegen Symmetrie
    for (const auto &point_elem_1 : point_map_1)
    {
        const std::vector<T> &id_vec_1 = point_elem_1.second;
        for (const auto &point_elem_2 : point_map_2)
        {
            std::uint16_t similarity_ct{0u};
            const std::vector<T> &id_vec_2 = point_elem_2.second;
            for (const auto &elem_1 : id_vec_1)
            {
                if (elem_1 == 0) 
                {
                    continue;
                }
                for (const auto &elem_2 : id_vec_2)
                {
                    float diff = static_cast<float>(elem_1 - elem_2);
                    if (std::abs(diff) < SIMILARITY_THRES)
                    {
                        ++similarity_ct;
                    }
                }
            }
            if (similarity_ct > 10) // if 11 or more equal connections are found this point is part of a constellation found in another report
            {
                similar_pts.push_back(std::make_pair(point_elem_1.first, point_elem_2.first));
            }
        }
    }
    // std::cout << "Similarity counter: " << similarity_ct << std::endl;
    return similar_pts;
}

bool fill_chain(std::map<std::string, std::vector<std::string>> &trafo_map, const std::map<std::string, std::vector<std::string>> &r_t_assignment, const std::string cur_system)
{
    for (const auto &elem : r_t_assignment.at(cur_system))
    {
        if (trafo_map.find(elem) == trafo_map.end()) // trafo to this report is unknown
        {
            trafo_map[elem] = trafo_map[cur_system];
            trafo_map[elem].push_back(elem);
        }
    }

    return trafo_map.size() == r_t_assignment.size();
}

/**
 * @brief Get a chain of transformations necessary to transform one scanner report to a another system defined in target_system
 * 
 * @param r_t_assignment Map providing information which report can be transformed to which reports. The vector contains the name of other reports the current report can be transformed to
 * @param target_system Scanner system that shall be used as target coordinate system 
 * @return std::map<std::string, std::vector<std::string>> 
 */
std::map<std::string, std::vector<std::string>> get_transformation_chain(const std::map<std::string, std::vector<std::string>> &r_t_assignment, const std::string target_system)
{
    std::map<std::string, std::vector<std::string>> trafo_map;
    trafo_map[target_system] = {target_system};
    size_t num_steps{ 0 };
    std::set<std::string> cur_reachable_systems = {target_system};

    while (num_steps < 20)
    {
        std::set<std::string> reachable_systems;
        for (const auto &system : cur_reachable_systems)
        {
            if (fill_chain(trafo_map, r_t_assignment, system))
            {
                break;
            }
            for (const auto &successor : r_t_assignment.at(system))
            {
                reachable_systems.insert(successor);
            }
        }
        ++num_steps;
        cur_reachable_systems = reachable_systems;
    }
    return trafo_map;
}

/**
 * @brief Transform all points given in scan_map to the coordinate system given in target_system
 * 
 * @param scan_map 
 * @param trafo_map Provides info about which report can be directly transformed to which other report 
 * @param report_trafo_assignment Provides info about the transformation matrix in order to transform points from one system to another
 * @param target_system 
 * @return std::map<std::string, std::vector<Point3D<tCoord>>> 
 */
std::map<std::string, std::vector<Point3D<tCoord>>> transform_pts(
    const std::map<std::string, std::vector<Point3D<tCoord>>> &scan_map, 
    const std::map<std::string, std::vector<std::string>> &trafo_map, 
    const std::map<std::string, std::map<std::string,Trafo>> &report_trafo_assignment, 
    const std::string target_system)
{
    std::map<std::string, std::vector<Point3D<tCoord>>> transformed_scan_map;
    transformed_scan_map[target_system] = scan_map.at(target_system);

    for (const auto &rep_trafo_vec : trafo_map)
    {
        if (rep_trafo_vec.second.size() == 1) // vector for target system has size of 1
        {
            continue;
        }
        std::string start_system = rep_trafo_vec.first;
        const std::vector<std::string> &trafos = rep_trafo_vec.second;
        std::vector<Point3D<tCoord>> cur_pts = scan_map.at(start_system);
        for (size_t i=trafos.size()-1; i>0; --i)
        {
            // Would have been faster to multiply transformation matrices and only transform points with final matrix
             cur_pts = transform_points(report_trafo_assignment.at(trafos.at(i-1)).at(trafos.at(i)), cur_pts);
        }
        transformed_scan_map[start_system] = cur_pts;
    }
    return transformed_scan_map;
}

}