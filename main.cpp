#include <fstream>
#include <chrono>
#include <iostream>
#include <json.hpp>
#include "src/auxiliary.hpp"
#include "src/database.hpp"

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
    // database db("db");
    // cout << db.get_collection("col1")->get_number_of_documents() << endl;

  
    fs::path p = fs::path("test/col2/t2.json");
    json obj = read_and_parse_json(p)[0];
    obj = access_nested_fields(obj, {});
    cout << obj;

  return 0;
}