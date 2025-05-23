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

    database db("db", "../dataset");
    collection* col = db.get_collection("movies");

    //CREATE
    json new_film = {
        {"title", "Tenet"},
        {"director", "Christopher Nolan"},
        {"year", 2020},
        {"genres", {"Action", "Sci-Fi"}}
        };
      
        unsigned long long doc_id = col->get_number_of_documents(); 
        cout << "Before: " << col->get_number_of_documents() << endl;
        col->create_document(new_film);
        cout << "After: " << col->get_number_of_documents() << endl;

    //READ 
    std::string field = "title";
    std::string value = "Tenet";
    std::vector<std::string> field_vector = {field};
    std::vector<json> read_results = col->read(field_vector, value);
    
    if (!read_results.empty()) {
        for (const auto &doc : read_results) {
            std::cout << "Original document:" << std::endl;
            std::cout << doc.dump(4) << std::endl;
        }
    } else {
        cout << "No documents found with " << field << " = " << value << endl;
    }

    //UPDATE
    json updated_film = {
        {"title", "Tenet"},
        {"director", "Christopher Nolan"},
        {"year", 2020},
        {"genres", {"Action", "Sci-Fi", "Thriller"}} // Only update existing fields
    };

    cout << "\n--- Testing Update ---" << endl;
    int update_result = col->update_document(doc_id, updated_film);
    
    if (update_result == 0) {
        cout << "Document successfully updated" << endl;
        
        // Read the updated document
        json updated_doc = col->get_document(doc_id);
        if (!updated_doc.empty()) {
            cout << "Updated document:" << endl;
            cout << updated_doc.dump(4) << endl;
        } else {
            cout << "Failed to retrieve updated document" << endl;
        }
    } else {
        cout << "Failed to update document. Error code: " << update_result << endl;
    }

    //DELETE
    int delete_count = col->delete_document(field, value);
    
    if (delete_count > 0) {
        cout << "\nSuccessfully deleted " << delete_count << " document" << endl;
        cout << "Document count after deletion " << col->get_number_of_documents() << endl;
    } else {
        cout << "Error deleting documents" << delete_count << endl;
    }

    //READ after deletion
    std::vector<json> verify_results = col->read(field_vector, value);

    if (verify_results.empty()) {
        cout << "No documents found after deletion" << endl;
    } else {
        cout << "Found " << verify_results.size() << " documents that should have been deleted" << endl;
    }


  return 0;

  
}