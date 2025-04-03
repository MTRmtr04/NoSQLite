#include <fstream>
#include <chrono>
#include <iostream>
#include "json.hpp"
#include "src/auxiliary.hpp"
#include "src/database.hpp"

using namespace std;
using namespace std::chrono;
using namespace nosqlite;
using json = nlohmann::json;


int main() {


    // std::ifstream file("../twitter.json");
    // auto start = high_resolution_clock::now();
    // json o;
    // file >> o;
    // auto stop = high_resolution_clock::now();
    // auto duration = duration_cast<microseconds>(stop - start);
    // file.close();

    // cout << "first " << duration.count() << endl;

    // start = high_resolution_clock::now();
    // ifstream file2("../twitter.json");
    // json j = json::parse(file2);
    // stop = high_resolution_clock::now();
    // file.close();
    // duration = duration_cast<microseconds>(stop - start);
    // cout << "second " << duration.count() << endl;

    database db("db", "../dataset");
    collection* col = db.get_collection("data_movies");

    unsigned long long id = 0;

    std::string hash = hash_integer(id);
    std::cout << "Hash do ID " << id << ": " << hash << std::endl;

    json doc = col->get_document(0);
    if(!doc.empty()){
      cout << doc.dump(4) << endl;
    } else {
      cout << "Document not found!" << endl;
    }

  return 0;
}