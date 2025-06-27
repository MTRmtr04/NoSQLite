#include <fstream>
#include <chrono>
#include <iostream>
#include "nosqlite.hpp"

using namespace std;
using namespace std::chrono;
using namespace nosqlite;

/**
 * This code is a simple showcase of the functionalities of NoSQLite.
 * Compilation instructions are in the README.md file in the project repository (https://github.com/MTRmtr04/NoSQLite).
 * This project uses an external library for manipulating JSON, this library can be found at (https://github.com/nlohmann/json)
 * 
 * This code receives two arguments from the user.
 * The first chooses which operation to perform in the switch case.
 * The second if it is 0, parallel processing with OpenMP is turned off. Any other value turn it on (although the default is with parallel processing turned on).
 * 
 * This project was developed by four students from the Bachelor's Degree in Computer and Informatics Engineering of Faculty of Engineering of the University of Porto.
 * The project was developed for the third year, second semester Capstone Project course.
 * Authors: Mateus Lima, Max Ribeiro, Sara Azevedo, Tiago Oliveira.
 */


int main(int argc, char** argv) {

    if (argc != 3) {
        cout << "USAGE: ./nosqlite <OPERATION> <PARALLEL FLAG>" << endl;
        return 1;
    }
    auto start1 = high_resolution_clock::now();
    nosqlite_api api("db");
    // nosqlite_api api("db", "data");
    auto stop1 = high_resolution_clock::now();
    auto duration1 = duration_cast<milliseconds>(stop1 - start1);
    cout << "Time taken for build: " << duration1.count() << "ms" << endl;
            
    vector<json> result = {};

    if (atoi(argv[2]) == 0) api.turn_off_parallel_processing();
    else api.turn_on_parallel_processing();

    switch (atoi(argv[1])) {
        case 1: {
            // Create and then delete the movie 'Memories of Murder' from the database.
            // Some reads in between to show that the movie is actually created and deleted.
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
            // Fetches the movies with the 'title' 'The Avengers' from the database.

            auto start = high_resolution_clock::now();
            api.read("movies", {
                {"title"}, "==", "The Avengers"
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
            // Updates the 'runtime' to 42 of any movie in the database with 'title' 'The Avengers'.

            auto start = high_resolution_clock::now();
            api.update("movies", {
                {"runtime", 42},
            },{
                {"title"}, "==", "The Avengers"
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
            // Creates a hash index on the attribute 'year' of the collection 'movies'.

            auto start = high_resolution_clock::now();
            api.create_index("movies", {"year"})->execute(result);
            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<milliseconds>(stop - start);
            cout << "Time taken for index creation: " << duration.count() << "ms" << endl;
            break;
        }
        case 5: {
            // Deletes the hash index on the attribute 'year' of the collection 'movies' if it exists.

            auto start = high_resolution_clock::now();
            api.delete_index("movies", {"year"})->execute(result);
            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<milliseconds>(stop - start);
            cout << "Time taken for index deletion: " << duration.count() << "ms" << endl;
            break;
        }
        case 6: {
            // Creates a hash index on the attribute 'title' of the collection 'movies'.

            auto start = high_resolution_clock::now();
            api.create_index("movies", {"title"})->execute(result);
            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<milliseconds>(stop - start);
            cout << "Time taken for index creation: " << duration.count() << "ms" << endl;
            break;
        }
        case 7: {
            // Deletes the hash index on the attribute 'title' of the collection 'movies' if it exists.

            auto start = high_resolution_clock::now();
            api.delete_index("movies", {"title"})->execute(result);
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
