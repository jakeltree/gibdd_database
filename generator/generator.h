#ifndef GENERATOR_GENERATOR_H_
#define GENERATOR_GENERATOR_H_

#include <chrono>
#include <string>
#include <vector>
#include <random>
#include <fstream>

namespace generator {
int Rand(int n); 

void Path2Vec(std::vector<std::string> &v, std::string path) ;

std::string Transform(int a, int n);

std::string GenerateARandomSign();

void PrintOffset(int offset_days);

void GenerateARandomFine();

void ProduceResult();


} //namespace generator 

#endif  // GENERATOR_GENERATOR_H_ 