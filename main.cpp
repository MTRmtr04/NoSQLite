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


    database db("movies");
    collection* col = db.get_collection("data_movies");


    vector<string> field = {"tomatoes", "viewer", "rating"};
    double value = 3.8;

    db.create_hash_index("movies", field);

    std::vector<json> results = col->read(field, value);

    unsigned long long id = 10;
    json before = col->get_document(id);
    std::cout << "Before the update:\n" << before.dump(4) << "\n";
    json update_fields = {
        {"year", 2024},
        // {"director", "New Director Name"}
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
