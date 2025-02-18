#include "collection.hpp"
#include "auxiliary.hpp"
#include <fstream>
#include <iostream>
#include <json.hpp>

using namespace nosqlite;
using json = nlohmann::json;


collection::collection(const std::string &path) : path(path) {
    // TODO: Get the number of documents from the collection header.
}

collection::collection(const std::string &path, const std::string &path_to_json) : path(path), number_of_documents(0) {
    this->build_from_scratch(path_to_json);
}

collection::~collection() {
    // TODO: Everything necessary.
}

void collection::build_from_scratch(const std::string &path_to_json) {
    
    // Checks if the directory with the json files to create the database with exists.
    if (int ret = check_path_existence(path_to_json) != 0) {
        std::cerr << "    The directory with the files to create the collection doesn't exist." << std::endl;
        return;
    }

    // Delete everything from the directory where the collection will be stored if it exists, else create it.
    fs::path path_to_collection = this->path;
    if (fs::exists(path_to_collection))
        for (const fs::path &entry : fs::directory_iterator(path_to_collection))
            fs::remove_all(entry);
    else fs::create_directory(path_to_collection);

    // Get all the file paths in the path_to_json directory
    std::vector<fs::path> paths;
    for (fs::path p : fs::recursive_directory_iterator(path_to_json)) {
        if (fs::is_directory(p)) continue;
        paths.push_back(p);
    }

    // Read all the json objects in the files and create a document for them in the database.
    for (fs::path p : paths) {
        // TODO: Create the documents for every JSON object in the files.
        // TODO: Output any error if any of the files are unreadable or if any of the documents can't be created.
    }




    // TODO: Build collection itself.
    // TODO: Build collection's header file.
    // TODO: Build collection's indices. 
}

std::string collection::get_path() const {
    return this->path;
}

std::string collection::get_name() const {
    return get_last_dir(this->path);
}

unsigned long long collection::get_number_of_documents() const {
    return this->number_of_documents;
}

int collection::add_document(const std::string &json_content, bool update_header) {

    int ret = 0;

    // Create the document and add the id field.
    json document = json::parse(json_content);
    document["id"] = this->number_of_documents;

    // TODO: Update indices

    // Hash the id.
    std::string id_hash = hash_integer(number_of_documents);

    // Build the path for the file according to the id hash.
    fs::path path_to_document = fs::path(this->path) / id_hash.substr(0, 2) / id_hash.substr(2, 2) / id_hash.substr(4).append(".json");

    // Create a new one or edit the file if it already exists (in case of a hash collision).
    if (fs::exists(path_to_document)) {
        std::fstream file(path_to_document);
        if (file.is_open()) {
            json json_file;
            file >> json_file;
            json_file.push_back(document);
            file << json_file << std::endl;
            file.close();
        }
        else {
            std::cerr << "Error: Failed to open file: \"" << path_to_document << "\"." << std::endl;
            ret = 1;
            update_header = false;
        }
    }
    else {
        std::ofstream file(path_to_document);
        if (file.is_open()) {
            file << '[' << document << ']' << std::endl;
            file.close();
        }
        else {
            std::cerr << "Error: Failed to create file: \"" << path_to_document << "\"." << std::endl;
            ret = 1;
            update_header = false;
        }
    }


    // Once everything else is successful increment the number of documents.
    if (update_header) {
        std::fstream header(fs::path(this->path) / "header.json");
        if (header.is_open()) {
            json h;
            header >> h;
            h["number_of_documents"] = this->number_of_documents + 1;
            header << h;
            header.close();
            this->number_of_documents++;
        }
        else {
            std::cerr << "Error: Failed to update collection header. Collection: \"" << this->get_name() << "\"." << std::endl;
            ret = 1;
            // TODO: Roll back changes if header update fails. CHECK WITH PROF
        }
    }
    else  this->number_of_documents++;

    return ret;
}

int collection::add_document(const std::string &json_content) {
    return this->add_document(json_content, true);
}