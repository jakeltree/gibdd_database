#pragma once

#include <string>

namespace gibdd_generator {
    std::string GenerateARandomSign();
    std::string GetDateOffset(int offset_days);
    std::string GenerateARandomFine();
}