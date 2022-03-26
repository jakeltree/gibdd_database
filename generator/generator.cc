#include <chrono>
#include <string>
#include <iostream>
#include <vector>
#include <random>
#include <fstream>

#include "date.h"
#include "generator.h"

#define sz(a) ((int)((a).size()))

namespace generator {

std::mt19937 rnd(std::chrono::steady_clock::now().time_since_epoch().count());
std::random_device dev;
std::mt19937 rng(dev());
int Rand(int n) {
  std::uniform_int_distribution<std::mt19937::result_type> dist(0, n - 1);
  return dist(rng);
}

void Path2Vec(std::vector<std::string> &v, std::string path) {
  std::ifstream file(path);
  std::string temp;
  while (file >> temp) {
    v.push_back(temp);
  }
}

std::string Transform(int a, int n) {
  std::string s;
  for (int i = pow(10, n - 1); i >= 1; i /= 10) {
    s += std::to_string(a / i);
    a -= a / i * i;
  }
  return s;
}

std::string GenerateARandomSign() {
  std::vector<std::string> letters{"А", "В", "Е", "К", "М", "Н", "О", "Р", "С", "Т", "У", "Х"};
  return letters[Rand(12)] + Transform(Rand(990) + 10, 3) +
         letters[Rand(12)] + letters[Rand(12)] +
         Transform(Rand(96) + 1, 2);
}


void PrintOffset(int offset_days) {
  using namespace date;
  using namespace std;

  auto ref_date = sys_days{2022_y / 2 / 3};
  auto tp = ref_date - days{offset_days};
  auto dp = floor<days>(tp);
  auto ymd = year_month_day{dp};
  std::cout << ymd;
}

void GenerateARandomFine() {
  const int MAX_DAYS = 2 * 365;
  auto offset_days = Rand(MAX_DAYS);
  PrintOffset(offset_days);
  std::cout << " " << (Rand(25) + 1) * 200 << " ";
  PrintOffset(offset_days + 60);
}

void ProduceResult() {
  int t;
  std::cin >> t;
  std::vector<std::string> male_names, male_surnames, male_patronyms;
  std::vector<std::string> female_names, female_surnames, female_patronyms;
  std::vector<std::string> brands;
  Path2Vec(male_names, "data/male_names.txt");
  Path2Vec(male_surnames, "data/male_surnames.txt");
  Path2Vec(male_patronyms, "data/male_patronyms.txt");
  Path2Vec(female_names, "data/female_names.txt");
  Path2Vec(female_surnames, "data/female_surnames.txt");
  Path2Vec(female_patronyms, "data/female_patronyms.txt");
  Path2Vec(brands, "data/brands.txt");

  while (t--) {
    std::string FIOnBrand;
    std::string sign;
    if (Rand(1)) {
      FIOnBrand = male_names[Rand(sz(male_names))] + " " +
                  male_surnames[Rand(sz(male_surnames))] + " " +
                  male_patronyms[Rand(sz(male_patronyms))] + "," +
                  brands[Rand(sz(brands))];
      sign = GenerateARandomSign();
    }
    else {
      FIOnBrand = female_names[Rand(sz(female_names))] + " " +
                  female_surnames[Rand(sz(female_surnames))] + " " +
                  female_patronyms[Rand(sz(female_patronyms))] + "," +
                  brands[Rand(sz(brands))];
      sign = GenerateARandomSign();
    }
    const int MAX_ENTRIES_PER_PERSON = 40;
    int entries_per_person = Rand(MAX_ENTRIES_PER_PERSON) + 1;
    while (entries_per_person--) {
      std::cout << FIOnBrand << ",";
      std::cout << sign << ",";
      GenerateARandomFine();
      std::cout << "," << std::endl;
    }
  }
}


} //namespace generator 

int main() {
  generator::ProduceResult();
  std::cerr << std::endl << "finished in " <<
    clock() * 1.0 / CLOCKS_PER_SEC << " sec" << std::endl;
  return 0;
}
