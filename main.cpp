#include <fstream>
#include <chrono>
#include <iostream>
#include <json.hpp>
#include "src/auxiliary.hpp"
#include "src/database.hpp"
#include "src/collection.hpp"

namespace fs = std::filesystem;
using namespace std;
using namespace std::chrono;
using namespace nosqlite;
using json = nlohmann::json;


int main() {


    // auto start = high_resolution_clock::now();

    // auto stop = high_resolution_clock::now();
    // auto duration = duration_cast<microseconds>(stop - start);

    // cout << "first " << duration.count() << endl;


    database db("db");
    collection* col = db.get_collection("data_movies");


    vector<string> field = {"tomatoes", "viewer", "rating"};
    double value = 3.8;
    
    db.create_hash_index("movies", field);

    std::vector<json> results = col->read(field, value);

    return 0;
}