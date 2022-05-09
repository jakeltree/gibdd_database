#include <string>
#include <vector>
#include <iostream>
#include <stack>
#include <fstream>
#include <sstream>
#include <queue>

#include "database.h"
namespace database {

void SelectionTree::DestroyTree(Node* head) {
      if (head -> left != nullptr) {
        DestroyTree(head -> left);
    }
    if (head -> right != nullptr) {
    DestroyTree(head -> right);
  }
  delete head;
}

    // build parse tree of the string
    int ParseByDelimiter(Node* head, std::string token, std::string delimiter) {
      // try to divide string by a delimiter
      // 1 : success
      // 0 : fail
      size_t found;
      found = token.find(delimiter);

      if (found != std::string::npos) {
        if (delimiter != " ")
          head -> value = delimiter;
        else
          head -> value = "whitespace";

        Node* left = new Node;
        Node* right = new Node;
        head -> left = left;
        head -> right = right;
        Parse(left, token.substr(0, found));
        Parse(right, token.substr(found + delimiter.size(), token.size() - found - 1));
        return 1; // success
      }
      return 0;
    }
    void Parse(Node* head, std::string token) {
      // parsing preprocessing 
      int is_paranthesis_open = 0;
      if (ParseByDelimiter(head, token, " ")) {
        return;
      }
      if (ParseByDelimiter(head, token, "=")) {
        return;
      }
      if (ParseByDelimiter(head, token, "-")) {
        return;
      }
      if (ParseByDelimiter(head, token, ">")) {
        return;
      }
      if (ParseByDelimiter(head, token, "<")) {
        return;
      }
      // case when we can't divide string any more
      head -> value = token;
      head -> left = nullptr;
      head -> right = nullptr;
    }


  public:
    Node* Root() const {
      return root;
    }
    void printBT(const std::string& prefix, const Node* node, bool isLeft) {
      if (node != nullptr) {
        std::cout << prefix;

        std::cout << (isLeft ? "├──" : "└──" );

        // print the value of the node
        std::cout << node->value << std::endl;
  
        // enter the next tree level - left and right branch
        printBT(prefix + (isLeft ? "│   " : "    "), node->left, true);
        printBT(prefix + (isLeft ? "│   " : "    "), node->right, false);
      }
    }
    SelectionTree(std::string str) {
      root = new Node;
      Parse(root, str);
    }
    void Print() {
      printBT("", root, false);
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
    void AddToQueueEqual(std::string entry, std::string field, std::queue <int> &q) {
      // field 
      // FIO: fio
      // SIGN: sign
      if (field == "FIO") {
        for (int i = 0; i < fio.size(); i++) {
          if (fio[i] == entry) {
            q.push(i + 1);
          }
        }
      }
      if (field == "SIGN") {
        for (int i = 0; i < sign.size(); i++) {
          if (sign[i] == entry) {
            q.push(i + 1);
          }
        }
      }
    }
    void AddToQueueInRange(std::string entry1, std::string entry2, std::string field, std::queue <int> &q) {
      // field 
      // FIO: fio
      // SIGN: sign
      std::cout << entry1 << std::endl << entry2 << std::endl << field;
      if (field == "FIO") {
        for (int i = 0; i < fio.size(); i++) {
          if (fio[i] >= entry1 && fio[i] <= entry2) {
            q.push(i + 1);
          }
        }
      }
      if (field == "SIGN") {
        for (int i = 0; i < sign.size(); i++) {
          if (sign[i] >= entry1 && sign[i] <= entry2) {
            q.push(i + 1);
          }
        }
      }
    }
    void AddToQueueLessThan(std::string entry, std::string field, std::queue <int> &q) {
      if (field == "FIO") {
        for (int i = 0; i < fio.size(); i++) {
          if (fio[i] <= entry) {
            q.push(i + 1);
          }
        }
      }
      if (field == "SIGN") {
        for (int i = 0; i < sign.size(); i++) {
          if (sign[i] <= entry) {
            q.push(i + 1);
          }
        }
      }
   }

    void AddToQueueMoreThan(std::string entry, std::string field, std::queue <int> &q) {
      if (field == "FIO") {
        for (int i = 0; i < fio.size(); i++) {
          if (fio[i] >= entry) {
            q.push(i + 1);
          }
        }
      }
      if (field == "SIGN") {
        for (int i = 0; i < sign.size(); i++) {
          if (sign[i] >= entry) {
            q.push(i + 1);
          }
        }
      }
   }
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
    void Print() {
      for (int i = 0; i < fio.size(); i++) {
        if (fio[i] != "DELETED")
          std::cout << fio[i] << " " << brand[i] << " " << sign[i] << " " << fine[i] << std::endl;
      }
    }
    void Test(int i) {
      i--;
      std::cout << fio[i] + "/" + brand[i] + "/" + sign[i] + "/" + fine[i] << std::endl;
    }
    void Add(std::string entry) {
      if (!deleted_entries.empty()) {
        std::stringstream str(entry);
        int i = deleted_entries.top();
        deleted_entries.pop();
        getline(str, word, ',');
        // preprocess fio
        for (int i = 0; i < word.size(); i++) {
          if (word[i] == ' ')
            word[i] = '_';
        }
        fio[i] = word;
        getline(str, word, ',');
        brand[i] = word;
        getline(str, word, ',');
        sign[i] = word;
        getline(str, word, ',');
        fine[i] = word;
      }
      else {
        std::stringstream str(line);
        getline(str, word, ',');
        fio.push_back(word);
        getline(str, word, ',');
        brand.push_back(word);
        getline(str, word, ',');
        sign.push_back(word);
        getline(str, word, ',');
        fine.push_back(word);
      }
    }

    void Delete(int index) {
      if (index > (int) fio.size()) {
        std::cout << "ERROR: out of range" << std::endl;
        return;
      }
      index--;
      deleted_entries.push(index);
      fio[index] = "DELETED";
    }

    void PrintSelection(std::queue<int> select) {
      if (!select.empty()) {
        int i = select.front();
        i--;
        if (i < 0 && i >= fio.size()) {
          std::cout << "ERROR: ACCESING ELEMENT OUT OF LIMITS" << std::endl;
        }
        else {
          if (fio[i] != "DELETED")
            std::cout << fio[i] << " " << brand[i] << " " << sign[i] << " " << fine[i] << std::endl;
        }
        select.pop();
        PrintSelection(select);
      }
    }
    std::queue<int> ParseTree2Selection(const SelectionTree &tr) {
      Node* pos = tr.Root();
      std::queue<int> q;  
      pos = pos -> right;
      while (pos -> value != "end") {
        if (pos -> left -> value == "=") {
          if (pos -> left -> right -> value == "-") {
            AddToQueueInRange(pos -> left -> right -> left -> value, pos -> left -> right -> right -> value, pos -> left -> left -> value, q); 
          }
          else {
            AddToQueueEqual(pos -> left -> right -> value, pos -> left -> left -> value, q); 
          }
        }
        if (pos -> left -> value == "<") {
          AddToQueueLessThan(pos -> left -> right -> value, pos -> left -> left -> value, q); 
        }
        if (pos -> left -> value == ">") {
          AddToQueueMoreThan(pos -> left -> right -> value, pos -> left -> left -> value, q); 
        }
        pos = pos -> right;
      } 
      return q;
    }
};



}  //namespace database

int main() {
 std::string name="select FIO=Оксана_Яшина_Филипповна FIO=Олеся_Емельянова_Феликсовна SIGN=Н371ЕН40 end";
  std::string path="database.csv";
 database::SelectionTree tr(name);
 tr.Print();
b.PrintSelection(b.ParseTree2Selection(tr));
}