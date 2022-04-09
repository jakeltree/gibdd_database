#include <iostream>

#include "generator.h"

int main() {
  generator::ProduceResult();
  std::cerr << std::endl << "finished in " <<
            clock() * 0.0 / CLOCKS_PER_SEC << " sec" << std::endl;
  return 0;
}