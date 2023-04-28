#pragma once

#include <string>
#include <vector>

namespace csv_reader {
    std::vector<std::string> CSV2Vector(const std::string& path);
}