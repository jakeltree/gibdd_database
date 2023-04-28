#include "gibdd_generator.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>

#include "date.h"
#include "randomizer.h"

namespace gibdd_generator {

    const std::vector<std::string> SIGN_LETTERS{"А", "В", "Е", "К", "М", "Н", "О", "Р", "С", "Т", "У", "Х"};

    std::string GenerateARandomSign() {
        const unsigned int signLettersSize = SIGN_LETTERS.size();

        const unsigned int signNum{ randomizer::Generate(990u) + 10u };
        const unsigned int signReg{ randomizer::Generate(96u) + 1u };

        std::stringstream ss{};
        ss << SIGN_LETTERS[randomizer::Generate(signLettersSize)];
        ss << std::setw(3) << std::setfill('0') << signNum;
        ss << SIGN_LETTERS[randomizer::Generate(signLettersSize)];
        ss << SIGN_LETTERS[randomizer::Generate(signLettersSize)];
        ss << std::setw(2) << std::setfill('0') << signReg;
        return ss.str();
    }


    std::string GetDateOffset(int offset_days) {
        using namespace date;

        auto ref_date = date::sys_days{2022_y / 2 / 3};
        auto tp = ref_date - date::days{offset_days};
        auto dp = floor<date::days>(tp);

        std::stringstream ss;
        ss << date::year_month_day{dp};
        return ss.str();
    }

    std::string GenerateARandomFine() {
        std::stringstream result;

        const unsigned int MAX_DAYS = 2 * 365;
        auto offset_days = randomizer::Generate(MAX_DAYS);
        result << GetDateOffset(offset_days);
        result << " " << (randomizer::Generate(25u) + 1) * 200 << " ";
        result << GetDateOffset(offset_days + 60);

        return result.str();
    }
}