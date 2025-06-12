#include <fstream>
#include <chrono>
#include <iostream>
#include <json.hpp>
#include "src/auxiliary.hpp"
#include "src/database.hpp"
#include "src/collection.hpp"
#include "src/nosqlite.hpp"
#include <unistd.h>

namespace fs = std::filesystem;
using namespace std;
using namespace std::chrono;
using namespace nosqlite;
using json = nlohmann::json;


int main(int argc, char** argv) {
    
    nosqlite_api api("db");
    vector<json> result = {};

    switch (atoi(argv[1])) {
        case 1: {
            // Read
            nosqlite_api api("db", "json_files");
            break;
        }
        case 2: {
            // Use an existing database.
            nosqlite_api api("db");
            break;
        }
        default: {
            cerr << "Invalid argument. Use 1 to create a new database or 2 to use an existing one." << endl;
            return 1;
        }
    }

/*
    std::vector<condition_type> conditions = {
        {{"year"}, ">", 2008},
        {{"year"}, "<", 2010},
        {{"imdb", "rating"}, ">", 6},
        {{"tomatoes","critic", "rating"}, ">", 7}
    };

    auto start = high_resolution_clock::now();
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "first " << duration.count() << endl;
*/
    api.read("movies", {
        {"year"}, "==", 2004
    })->AND({
        {"genres"}, "==", "Documentary"
    })->AND({
        {"countries"}, "==", "Germany"
    })->execute(result);
    for (const auto& doc : result) {
        cout << endl << endl << "-----" << doc["id"] << "-----" << endl << endl;
        cout << doc.dump(4) << endl;
    }
    cout << "Total documents: " << result.size() << endl;

    api.update("movies", {
        {"imdb", {
            {"rating", 9.5},
        }}
    },{
        {"year"}, "==", 2004
    })->AND({
        {"genres"}, "==", "Documentary"
    })->AND({
        {"countries"}, "==", "Germany"
    })->execute(result);

    for (const auto& doc : result) {
        cout << endl << endl << "-----" << doc["id"] << "-----" << endl << endl;
        cout << doc.dump(4) << endl;
    }
    cout << "Total documents: " << result.size() << endl;

    return 0;
}
