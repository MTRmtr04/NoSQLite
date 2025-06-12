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


int main() {

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
    nosqlite_api api("db");
    vector<json> result = {};
    // api.create_index("movies", {"year"})->execute(result);
    // sleep(5);
    // api.delete_index("movies", {"year"})->execute(result);
    // read movies from 2004
    api.read("movies", {
        {"year"}, "==", 2004
    })->AND({
        {"genres"}, "==", "Documentary"
    })->AND({
        {"countries"}, "==", "Germany"
    })->execute(result);
    for (const auto& doc : result) {
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
        cout << doc.dump(4) << endl;
    }
    cout << "Total documents: " << result.size() << endl;
    // database db("db");
    // db.build_from_existing();
    // collection* col = db.get_collection("movies");

/*
    // READ
   

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
    
    auto start = high_resolution_clock::now();

    // Delete the movie created
    // int deleted = col->delete_with_conditions({
    //     {{"imdb", "rating"}, "<", 1.0}
    // });
    int deleted = col->delete_document({"imdb", "rating"}, 0.5);
    cout << "Deleted " << deleted << " movie" << endl;
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    cout << "first " << duration.count() << endl;
    
    // Verify deletion
    bool exists = !col->read_with_conditions({
        {{"title"}, "==", "Test Movie"}
    }).empty();

    cout << "Test movie still exists: " << (exists ? "YES" : "NO -> SUCCESS") << endl;
    cout << "Final count: " << col->get_number_of_documents() << endl;
*/

/*
    unsigned long long id = 10;
    json before = col->get_document(id);
    std::cout << "Before the update:\n" << before.dump(4) << "\n";
    json update_fields = {
        {"year", 2024},
        // {"director", "New Director Name"}
        {"imdb", {
            {"id", 7558},
            {"rating", 5.9},
            {"votes", 247}
        }}
    };

    int result = col->update_document(id, update_fields);
    if(result == 0){
        json after = col->get_document(10);
        std::cout << "After the update:\n" << after.dump(4) << std::endl;
    } else {
        cout << "Error on updating the document!" << endl;
    }
*/

/*
    std::vector<json> all_docs = col->read_all();

    for (const auto& doc : all_docs) {
        std::cout << doc.dump(4) << std::endl;
    }
    std::cout << "Total documents: " << all_docs.size() << std::endl;
*/

    return 0;
}
