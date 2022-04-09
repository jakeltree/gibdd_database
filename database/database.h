#ifndef DATABASE_DATABASE_H_
#define DATABASE_DATABASE_H_
class Seabase {
    std::vector<std::string> fio, brand, sign, fine;
    std::stack<int> deleted_entries;
  public:

    Seabase(std::string filepath = "database.csv") {
      std::vector<std::string> row;
      std::string line, word;
      std::ifstream file(filepath);
      if (file.is_open()) {
        while(getline(file, line)) {
          row.clear();
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
    }

    void Test(int i) {
      i--;
      std::cout << fio[i] + "/" + brand[i] + "/" + sign[i] + "/" + fine[i] << std::endl;
    }

    void Add(std::string entry, int index) {
      if (index > (int) fio.size()) {
        std::cout << "ERROR: out of the range" << std::endl;
        return;
      }
      index--;
      if (!deleted_entries.empty()) {
        std::stringstream str(entry);
        int i = deleted_entries.top();
        deleted_entries.pop();
        getline(str, word, ',');
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

    std::vector<int> Tok2Sel(std::string token);
    void Delete(int index) {
      if (index > (int) fio.size()) {
        std::cout << "ERROR: out of range" << std::endl;
        return;
      }
      index--;
      deleted_entries.push(index);
    }

    void PrintSelection(std::queue<int> select) {
      if (!select.empty()) {
        int i = select.front();
        i--;
        std::cout << fio[i] << " " << brand[i] << " " << sign[i] << " " << fine[i] << std::endl;
        select.pop();
        PrintSelection(select);
      }
    }
};

class SelectionTree {
  public:
    struct Node;
    Node* root;

    void DestroyTree(Node* head);
    void Parse(Node* head, std::string token);

  public:
    SelectionTree(std::string str) {
      root = new Node;
      Parse(root, str);
    }

    ~SelectionTree() {
      DestroyTree(root);
    }
};
#endif //DATABASE_DATABASE_H_
