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
    collection* col = db.get_collection("movies");

    // READ
    std::vector<std::tuple<std::vector<std::string>, std::string, json>> conditions = {
        {{"year"}, ">", 2008},
        {{"year"}, "<", 2010},
        {{"imdb", "rating"}, ">", 6},
        {{"tomatoes","critic", "rating"}, ">", 7}
    };

    auto docs = col->read_with_conditions(conditions);

    if (docs.empty()) {
        cout << "Nenhum documento encontrado.\n";
    } else {
        int i = 1;
        cout << "Documentos encontrados:\n";
        for (const auto& doc : docs) {
            cout << "----------" << "Documento " << i << "----------" << std::endl;
            cout << doc.dump(4) << "\n\n";
            cout << "----------------------------------" << std::endl;
            i++;
        }
    }

    // DELETE 
    int initial_count = col->get_number_of_documents();
    cout << "Initial document count: " << initial_count << endl;

    json test_movie = {
        {"title", "Test Movie"},
        {"year", 2025},
        {"imdb", {{"rating", 0.5}}}
    };

    // Add the test movie
    col->create_document(test_movie);
    cout << "Current count: " << col->get_number_of_documents() << endl;

    // Delete the movie created
    int deleted = col->delete_with_conditions({
        {{"imdb", "rating"}, "<", 1.0}
    });
    cout << "Deleted " << deleted << " movie" << endl;

    // Verify deletion
    bool exists = !col->read_with_conditions({
        {{"title"}, "==", "Test Movie"}
    }).empty();

    cout << "Test movie still exists: " << (exists ? "YES" : "NO -> SUCCESS") << endl;
    cout << "Final count: " << col->get_number_of_documents() << endl;

    return 0;
}