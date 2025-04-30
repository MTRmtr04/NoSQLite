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
    // cout << obj;

    database db("db", "../dataset");
    collection* col = db.get_collection("data_movies");

/*
    //For testing Read 

    //std::string field = "genres";
    //std::string value = "Comedy";
    
    //std::string field = "released.$date";
    //std::string value = "2012-09-21T00:00:00Z";
 
    std::string field = "tomatoes.viewer.rating";
    double value = 3.8;
    
    //std::string field = "imdb.id";
    //int value = 1723121;

    std::vector<json> results = col->read(field, value);
    
    if (!results.empty()) {
        for (const auto &doc : results) {
            std::cout << doc.dump(4) << std::endl;
        }
    } else {
        std::cout << "No documents found with " << field << " = " << value << std::endl;
    }
*/

/*
    //For testing Create
    json new_film = {
      {"title", "Tenet"},
      {"director", "Christopher Nolan"},
      {"year", 2020},
      {"genres", {"Action", "Sci-Fi"}}
    };

    database db("db", "../dataset");
    collection* col = db.get_collection("data_movies");

    cout << "Before: " << col->get_number_of_documents() << endl;
    col->create_document(new_film);
    cout << "After: " << col->get_number_of_documents() << endl;
*/
    unsigned long long id = 10;
    json before = col->get_document(id);
    std::cout << "Before the update:\n" << before.dump(4) << "\n";

    json update_fields = {
        {"blablala", 2024},
        {"director", "New Director Name"}
    };

    int result = col->update_document(id, update_fields);
    if(result == 0){
        json after = col->get_document(10);
        std::cout << "After the update:\n" << after.dump(4) << std::endl;
    } else {
        cout << "Error on updating the document!" << endl;
    }

  return 0;
}