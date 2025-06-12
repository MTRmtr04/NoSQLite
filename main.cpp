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
            api.read("movies", {{"title"}, "==", "Memories of Murder"})->execute(result);
            cout << "Number of documents: " << result.size() << endl;

            auto start = high_resolution_clock::now();
            api.create("movies", {
                {"title", "Memories of Murder"},
                {"director", "Bong Joon Ho"},
                {"year", 2003},
                {"imdb", {"rating", 8.1}}
            })->execute(result);
            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(stop - start);
            cout << "Time taken for create: " << duration.count() << endl;
            
            api.read("movies", {{"title"}, "==", "Memories of Murder"})->execute(result);
            cout << "Number of documents: " << result.size() << endl;
            cout << result[0].dump(4) << endl;
            result = {};

            start = high_resolution_clock::now();
            //TODO: delete.
            stop = high_resolution_clock::now();
            duration = duration_cast<microseconds>(stop - start);
            cout << "Time taken for delete: " << duration.count() << endl;

            api.read("movies", {{"title"}, "==", "Memories of Murder"})->execute(result);
            cout << "Number of documents: " << result.size() << endl;
            break;
        }
        case 2: {
            // Read
            auto start = high_resolution_clock::now();
            api.read("movies", {
                {"year"}, "==", 2004
            })->AND({
                {"genres"}, "==", "Documentary"
            })->AND({
                {"countries"}, "==", "Germany"
            })->execute(result);
            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(stop - start);

            for (const auto& doc : result) {
                cout << endl << endl << "-----" << doc["id"] << "-----" << endl << endl;
                cout << doc.dump(4) << endl;
            }
            cout << "Total documents: " << result.size() << endl;
            cout << "Time taken for read: " << duration.count() << endl;
            break;
        }
        case 3: {
            // Update
            auto start = high_resolution_clock::now();
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
            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(stop - start);

            for (const auto& doc : result) {
                cout << endl << endl << "-----" << doc["id"] << "-----" << endl << endl;
                cout << doc.dump(4) << endl;
            }
            cout << "Total documents: " << result.size() << endl;
            cout << "Time taken for update: " << duration.count() << endl;
            break;
        }
        case 4: {
            auto start = high_resolution_clock::now();
            api.create_index("movies", {"year"})->execute(result);
            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(stop - start);
            cout << "Time taken for index creation: " << duration.count() << endl;
            break;
        }
        default: {
            cerr << "Invalid argument." << endl;
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
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "first " << duration.count() << endl;
*/
    return 0;
}
