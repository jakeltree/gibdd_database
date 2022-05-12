#include <string>
#include <vector>
#include <iostream>
#include <stack>
#include <fstream>
#include <sstream>
#include <queue>

namespace database {
class Seabase; 
struct Node {
  std::string value;
  Node *left, *right;
};

class SelectionTree {
  private:
    
    Node* root;

    void DestroyTree(Node* head) {
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
    void AddToQueueEqual(std::string entry, std::string field, std::queue <size_t> &q) {
      // field 
      // FIO: fio
      // SIGN: sign
      if (field == "FIO") {
        for (size_t i = 0; i < fio.size(); i++) {
          if (fio[i] == entry) {
            q.push(i + 1);
          }
        }
      }
      if (field == "SIGN") {
        for (size_t i = 0; i < sign.size(); i++) {
          if (sign[i] == entry) {
            q.push(i + 1);
          }
        }
      }
    }
    void AddToQueueInRange(std::string entry1, std::string entry2, std::string field, std::queue <size_t> &q) {
      // field 
      // FIO: fio
      // SIGN: sign
      std::cout << entry1 << std::endl << entry2 << std::endl << field;
      if (field == "FIO") {
        for (size_t i = 0; i < fio.size(); i++) {
          if (fio[i] >= entry1 && fio[i] <= entry2) {
            q.push(i + 1);
          }
        }
      }
      if (field == "SIGN") {
        for (size_t i = 0; i < sign.size(); i++) {
          if (sign[i] >= entry1 && sign[i] <= entry2) {
            q.push(i + 1);
          }
        }
      }
    }
    void AddToQueueLessThan(std::string entry, std::string field, std::queue <size_t> &q) {
      if (field == "FIO") {
        for (size_t i = 0; i < fio.size(); i++) {
          if (fio[i] <= entry) {
            q.push(i + 1);
          }
        }
      }
      if (field == "SIGN") {
        for (size_t i = 0; i < sign.size(); i++) {
          if (sign[i] <= entry) {
            q.push(i + 1);
          }
        }
      }
   }

    void AddToQueueMoreThan(std::string entry, std::string field, std::queue <size_t> &q) {
      if (field == "FIO") {
        for (size_t i = 0; i < fio.size(); i++) {
          if (fio[i] >= entry) {
            q.push(i + 1);
          }
        }
      }
      if (field == "SIGN") {
        for (size_t i = 0; i < sign.size(); i++) {
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
          for (size_t i = 0; i < word.size(); i++) {
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
      for (size_t i = 0; i < fio.size(); i++) {
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
        for (size_t i = 0; i < word.size(); i++) {
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


    void PrintSelection(std::queue<size_t> select) {
      while(!select.empty()) {
        size_t i = select.front();
        i--;
        if (i >= fio.size()) {
          std::cout << "ERROR: ACCESING ELEMENT OUT OF LIMITS" << std::endl;
        }
        else {
          if (fio[i] != "DELETED")
            std::cout << fio[i] << " " << brand[i] << " " << sign[i] << " " << fine[i] << std::endl;
        }
        select.pop();
      }
    }

    void SelectionToFile(std::queue<size_t> select, std::string path) {
      std::ofstream file(path);
      while (!select.empty()) {
        size_t i = select.front();
        i--;
        if (i >= fio.size()) {
          std::cout << "ERROR: ACCESSING ELEMENT OUT OF LIMITS" << std::endl;
        }
        else {
          if (fio[i] != "DELETED")
            file << fio[i] << " " << brand[i] << " " << sign[i] << " " << fine[i] << std::endl;
        }
        select.pop();
      }
      file.close();
    }
    std::queue<size_t> ParseTreeToSelection(const SelectionTree &tr) {
      Node* pos = tr.Root();
      std::queue<size_t> q;  
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
    void AddHTML(std::stringstream & html, std::queue<size_t> select) {
        while (!select.empty()) {
          size_t i = select.front();
          i--;
          std::vector<std::string> words;
          std::string whitespace = " ";
          size_t delim1 = fine[i].find(whitespace);
          size_t delim2 = fine[i].find(whitespace, delim1+1);
          words.push_back(fine[i].substr(0, delim1));
          words.push_back(fine[i].substr(delim1 + 1, delim2 - delim1));
          words.push_back(fine[i].substr(delim2 + 1, fine[i].size() - delim2));
          html << "<tr>";
          html << "<td>" + fio[i] + "</td>";
          html << "<td>" + brand[i] + "</td>";
          html << "<td>" + sign[i] + "</td>";
          html << "<td>" + words[0] + "</td>";
          html << "<td>" + words[2] + "</td>";
          html << "<td>" + words[1] + "</td>";
          html << "</tr>";
          select.pop();
        }
    }
};



}  //namespace database
