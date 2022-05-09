#ifndef DATABASE_DATABASE_H_
#include <string>
#include <vector>
#include <iostream>
#include <stack>
#include <fstream>
#include <sstream>
#include <queue>

namespace database {
struct Node {
  std::string value;
  Node *left, *right;
};

class SelectionTree {
  private:
    
    Node* root;

    void DestroyTree(Node* head); 
   

    // build parse tree of the string
    int ParseByDelimiter(Node* head, std::string token, std::string delimiter); 
    
    void Parse(Node* head, std::string token);

  public:
    Node* Root() const;    
    void printBT(const std::string& prefix, const Node* node, bool isLeft);
    SelectionTree(std::string str) 
      root = new Node;
      Parse(root, str);
    }
    // convert parse tree to the selection
    ~SelectionTree() {
      DestroyTree(root);
    }
};

class Seabase {
  private:
    std::vector<std::string> fio, brand, sign, fine;
    std::stack<int> deleted_entries;
    void AddToQueueEqual(std::string entry, std::string field, std::queue <int> &q);
    
    void AddToQueueInRange(std::string entry1, std::string entry2, std::string field, std::queue <int> &q);
    void AddToQueueLessThan(std::string entry, std::string field, std::queue <int> &q); 
    void AddToQueueMoreThan(std::string entry, std::string field, std::queue <int> &q); 
  public:
    std::vector<std::string> row;
    std::string line, word;

    Seabase(std::string filepath) {
      std::ifstream file(filepath);
      if (file.is_open()) {
        while(getline(file, line)) {
          row.clear();
          std::stringstream str(line); getline(str, word, ',');
          for (int i = 0; i < word.size(); i++) {
            if (word[i] == ' ')
              word[i] = '_';
            }
          fio.push_back(word);
          getline(str, word, ',');
          brand.push_back(word);
          getline(str, word, ',');
          sign.push_back(word);
          getline(str, word, ',');
          fine.push_back(word);
        }
      }
    } 
    void Print();
    void Test(int i) {
      i--;
      std::cout << fio[i] + "/" + brand[i] + "/" + sign[i] + "/" + fine[i] << std::endl;
    }
    void Add(std::string entry);

    void Delete(int index); 

    void PrintSelection(std::queue<int> select); 
    std::queue<int> ParseTree2Selection(const SelectionTree &tr); 
};



}  //namespace database

#define DATABASE_DATABASE_H_
#endif 