#include "csv_reader.h"
#include <iostream>
#include <fstream>

namespace csv_reader {
    std::vector<std::string> CSV2Vector(const std::string& path) {
        std::ifstream fileStream(path);
        if(!fileStream.is_open()) {
            std::cerr << "CSV2Vector error. File " << path << "doesn't exist!" << std::endl;
            return {};
        }

        std::vector<std::string> result;
        std::copy(std::istream_iterator<std::string>(fileStream),std::istream_iterator<std::string>(), std::back_inserter(result));
        return result;
    }
}