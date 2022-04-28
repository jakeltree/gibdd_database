#ifndef DATABASE_DATABASE_H_
#define DATABASE_DATABASE_H_
#include <string>
#include <vector>
#include <iostream>
#include <stack>
#include <fstream>
#include <sstream>
#include <queue>


namespace database {

class Seabase {
  private:
    std::vector<std::string> fio, brand, sign, fine;
    std::stack<int> deleted_entries;
  public:
    std::vector<std::string> row;
    std::string line, word;

    Seabase(std::string filepath);
    void Test(int i); 
    void Add(std::string entry, int index);
    void Delete(int index);
    void PrintSelection(std::queue<int> select);
};

class SelectionTree {
  private:
    struct Node;
    Node* root;

    void DestroyTree(Node* head);
    int ParseByDelimiter(Node* head, std::string token, std::string delimiter);
    void Parse(Node* head, std::string token);
    void printBT(const std::string& prefix, const Node* node, bool isLeft);


  public:
    SelectionTree(std::string str);
    void Print(); 
    ~SelectionTree();
};


}  //namespace database
#endif 