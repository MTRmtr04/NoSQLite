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

    if (argc != 3) {
        cout << "USAGE: ./nosqlite <OPERATION> <PARALLEL FLAG>" << endl;
        return 1;
    }
    auto start1 = high_resolution_clock::now();
    nosqlite_api api("db");
    auto stop1 = high_resolution_clock::now();
    auto duration1 = duration_cast<milliseconds>(stop1 - start1);
    cout << "Time taken for build: " << duration1.count() << "ms" << endl;
            
    vector<json> result = {};

    if (atoi(argv[2]) == 0) api.turn_off_parallel_processing();
    else api.turn_on_parallel_processing();

    switch (atoi(argv[1])) {
        case 1: {
            api.read("movies", {{"title"}, "==", "Memories of Murder"})->execute(result);
            cout << "Number of documents: " << result.size() << endl;

            auto start = high_resolution_clock::now();
            api.create("movies", {
                {"title", "Memories of Murder"},
                {"director", "Bong Joon Ho"},
                {"year", 2003},
                {"imdb", {{"rating", 8.1}}}
            })->execute(result);
            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<milliseconds>(stop - start);
            cout << "Time taken for create: " << duration.count() << "ms" << endl;
            
            api.read("movies", {{"title"}, "==", "Memories of Murder"})->execute(result);
            cout << "Number of documents: " << result.size() << endl;
            cout << result[0].dump(4) << endl;
            result = {};

            start = high_resolution_clock::now();
            api.remove("movies", {{"title"}, "==", "Memories of Murder"})->execute(result);
            stop = high_resolution_clock::now();
            duration = duration_cast<milliseconds>(stop - start);
            cout << "Number of documents deleted: " << result.size() << endl;
            cout << "Time taken for delete: " << duration.count() << "ms" << endl;

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
            })->AND({
                {"imdb", "rating"}, ">=", 7.6
            })->execute(result);
            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<milliseconds>(stop - start);

            for (const auto& doc : result) {
                cout << endl << endl << "-----" << doc["id"] << "-----" << endl << endl;
                cout << doc.dump(4) << endl;
            }
            cout << "Total documents: " << result.size() << endl;
            cout << "Time taken for read: " << duration.count() << "ms" << endl;
            break;
        }
        case 3: {
            // Update
            auto start = high_resolution_clock::now();
            api.update("movies", {
                {"runtime", 42},
            },{
                {"year"}, "==", 2004
            })->AND({
                {"genres"}, "==", "Documentary"
            })->AND({
                {"countries"}, "==", "Germany"
            })->AND({
                {"imdb", "rating"}, ">=", 7.6
            })->execute(result);
            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<milliseconds>(stop - start);

            for (const auto& doc : result) {
                cout << endl << endl << "-----" << doc["id"] << "-----" << endl << endl;
                cout << doc.dump(4) << endl;
            }
            cout << "Total documents: " << result.size() << endl;
            cout << "Time taken for update: " << duration.count() << "ms" << endl;
            break;
        }
        case 4: {
            auto start = high_resolution_clock::now();
            api.create_index("movies", {"year"})->execute(result);
            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<milliseconds>(stop - start);
            cout << "Time taken for index creation: " << duration.count() << "ms" << endl;
            break;
        }
        case 5: {
            auto start = high_resolution_clock::now();
            api.delete_index("movies", {"year"})->execute(result);
            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<milliseconds>(stop - start);
            cout << "Time taken for index deletion: " << duration.count() << "ms" << endl;
            break;
        }
        default: {
            cerr << "Invalid argument." << endl;
            return 1;
        }
    }
    return 0;
}
