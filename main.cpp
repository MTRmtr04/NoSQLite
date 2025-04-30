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
    //database db("movies", "../dataset");
    //db.get_collection("movies")->create_hash_index("title");
    // cout << db.get_collection("col1")->create_hash_index("a", "b", "c") << endl;

  
    // fs::path p = fs::path("test/col2/t2.json");
    // json obj = read_and_parse_json(p)[0];
    // obj["test"].push_back("help");
    // obj["test"].push_back("help");
    // cout << obj["test"];

    //For testing Read 
    auto start = high_resolution_clock::now();

    database db("db");
    collection* col = db.get_collection("movies");

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    cout << "first " << duration.count() << endl;

    vector<string> field = {"tomatoes", "viewer", "rating"};
    double value = 3.8;

    start = high_resolution_clock::now();
    
    db.create_hash_index("movies", field);

    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);

    cout << "second " << duration.count() << endl;

    start = high_resolution_clock::now();

    vector<json> x = col->read(field, value);

    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);

    cout << "third " << duration.count() << " number " << x.size() << endl;

  return 0;
}