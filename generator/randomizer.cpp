#include "randomizer.h"
#include <chrono>
#include <random>

namespace randomizer {
    std::mt19937 rnd(std::chrono::steady_clock::now().time_since_epoch().count());
    std::random_device dev;
    std::mt19937 rng(dev());

    unsigned int Generate(unsigned int n) {
        if(n <= 1) {
            return 0;
        }

        std::uniform_int_distribution<std::mt19937::result_type> dist(0, n - 1);
        return dist(rng);
    }
}