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
    auto start = high_resolution_clock::now();

    auto stop = high_resolution_clock::now();
    auto duration1 = duration_cast<microseconds>(stop - start);


    auto duration2 = duration_cast<microseconds>(stop - start);

    // object["search_metadata"]["count"] = 1000;

    std::fstream file("../lib/twitter.json");
    json o;
    file >> o;
    start = high_resolution_clock::now();

    ofstream g("test2.json");
    o["statusus"] = {"x",{"help"}};
    g << o;

    stop = high_resolution_clock::now();
    file.close();
    auto duration3 = duration_cast<microseconds>(stop - start);

    // std::cout << uint64_t(object["search_metadata"]["count"]) << " results." << std::endl;

    cout << "Time taken by function nlohmann: "
            << duration3.count() << " microseconds" << endl;


    // database db = database("db", "../src/test");

  return 0;
}