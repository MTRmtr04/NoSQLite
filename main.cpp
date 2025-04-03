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

    json new_film = {
      {"title", "Tenet"},
      {"director", "Christopher Nolan"},
      {"year", 2020},
      {"genres", {"Action", "Sci-Fi"}}
    };

    database db("db", "../dataset");
    collection* col = db.get_collection("data_movies");

    cout << "Before: " << col->get_number_of_documents() << endl;
    col->create_document(new_film);
    cout << "After: " << col->get_number_of_documents() << endl;

  return 0;
}