#include <string>

#include "dot.h"
#include "../utility.h"



unsigned long get_num_dots(const std::string& file_path)
{
    TransparencyDecoder decoder{file_path};
    decoder.execute_folds();
    decoder.print_dots(std::cout);
    return decoder.get_num_dots();
}


