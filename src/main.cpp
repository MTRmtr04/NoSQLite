#include <fstream>
#include <chrono>
#include "json.hpp"
#include "simdjson.h"
#include "json_parsing.hpp"

using namespace std;
using namespace std::chrono;
using namespace simdjson;
using json = nlohmann::json;


int main() {
    ondemand::parser parser;
    padded_string doc = padded_string::load("../lib/twitter.json");
    ondemand::document tweets = parser.iterate(doc);
    std::cout << uint64_t(tweets["search_metadata"]["count"]) << " results." << std::endl;
    auto start = high_resolution_clock::now();
    json object = simdjson_to_nlohmann(tweets);
    auto stop = high_resolution_clock::now();
    object["search_metadata"]["count"] = 1000;

    std::cout << uint64_t(object["search_metadata"]["count"]) << " results." << std::endl;
    auto duration = duration_cast<microseconds>(stop - start);

    cout << "Time taken by function: "
         << duration.count() << " microseconds" << endl;

  return 0;
}