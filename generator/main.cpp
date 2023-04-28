// Generate the CSV file with the "fake" GIBDD records
#include <iostream>
#include <fstream>

#include <string>
#include <vector>

#include "randomizer.h"
#include "gibdd_generator.h"
#include "csv_reader.h"

#define sz(a) ((unsigned int)((a).size()))
namespace generator {

std::string output_path = "database.csv";
const int NUM_PEOPLE = 1000;
const int ENTRIES_PER_PERSON = 40;

bool ProduceResult() {
    std::vector<std::string> male_names = csv_reader::CSV2Vector("data/male_names.csv");
    std::vector<std::string> male_surnames = csv_reader::CSV2Vector("data/male_surnames.csv");
    std::vector<std::string> male_patronyms = csv_reader::CSV2Vector("data/male_patronyms.csv");
    std::vector<std::string> female_names = csv_reader::CSV2Vector("data/female_names.csv");
    std::vector<std::string> female_surnames = csv_reader::CSV2Vector("data/female_surnames.csv");
    std::vector<std::string> female_patronyms = csv_reader::CSV2Vector("data/female_patronyms.csv");
    std::vector<std::string> brands = csv_reader::CSV2Vector("data/brands.csv");
    if(male_names.empty() || male_surnames.empty() || male_patronyms.empty() || female_names.empty() || female_surnames.empty() || female_patronyms.empty() || brands.empty()) {
        std::cerr << "Source generator's data doesn't exist!" << std::endl;
        return false;
    }

    std::ofstream output_file_stream(output_path);
    if(!output_file_stream) {
        std::cerr << "ProduceResult error. File " << output_path << "doesn't exist!" << std::endl;
        return false;
    }

    for (int i = 0; i < NUM_PEOPLE; i++) {
        std::string fio;

        const bool isMale = randomizer::Generate(2u) == 0;
        if (isMale) {
          fio = male_surnames[randomizer::Generate(sz(male_surnames))] + " " +
                      male_names[randomizer::Generate(sz(male_names))] + " " +
                      male_patronyms[randomizer::Generate(sz(male_patronyms))];
        }
        else {
          fio = female_surnames[randomizer::Generate(sz(female_surnames))] + " " +
                      female_names[randomizer::Generate(sz(female_names))] + " " +
                      female_patronyms[randomizer::Generate(sz(female_patronyms))];
        }
        std::string brand = brands[randomizer::Generate(sz(brands))];
        std::string sign = gibdd_generator::GenerateARandomSign();

        for (int j = 0; j < ENTRIES_PER_PERSON; j++) {
            output_file_stream << fio << "," << brand << "," << sign << ",";
            output_file_stream << gibdd_generator::GenerateARandomFine() << "," << std::endl;
        }
    }

    return true;
}


} //namespace generator

int main() {
  const bool result = generator::ProduceResult();
    std::cout << "Finished in " << clock() * 0.0 / CLOCKS_PER_SEC << " sec" << std::endl;

    if(result) {
      std::cout << "Successfully completed. Data was generated to " << generator::output_path << " file" << std::endl;
  } else {
      std::cerr << "Error happened. No output was generated." << std::endl;
      return 1;
  }
    return 0;
}