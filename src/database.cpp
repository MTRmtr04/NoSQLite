#include <iostream>
#include <filesystem>
#include <fstream>
#include "database.hpp"
#include "auxiliary.hpp"
#include <json.hpp>

using json = nlohmann::json;
using namespace nosqlite;
namespace fs = std::filesystem;

    
database::database(const std::string &path) : path(path) {
    this->build_from_existing();
}

database::database(const std::string &path_to_database, const std::string &path_to_json) : path(path_to_database) {
    this->build_from_scratch(path_to_json);
}

database::~database() {
    for (auto p : this->collections) {
        delete p.second;
    }
}

void database::build_from_scratch(const std::string &path_to_json) {

    // Checks if the directory with the json files to create the database with exists.
    if (int ret = check_path_existence(path_to_json) != 0) return;

    // Delete everything from the directory where the database will be stored if it exists, else create it.
    fs::path path_to_database = this->path;
    if (fs::exists(path_to_database))
        for (const fs::path &entry : fs::directory_iterator(path_to_database))
            fs::remove_all(entry);
    else fs::create_directories(path_to_database);

    
    std::vector<std::string> collection_names;
    // Create each individual collection
    for (const fs::path &entry : fs::directory_iterator(path_to_json)) {
        if (!fs::is_directory(entry)) continue;
        std::string collection_name = get_last_dir(entry.string());
        collection_names.push_back(collection_name);

        fs::path collection_path = fs::path(this->path) / collection_name;

        collection* col = new collection(collection_path.string(), (entry.parent_path() / collection_name).string());

        this->collections[collection_name] = col;
    }
    std::sort(collection_names.begin(), collection_names.end());


    // Creates the header file for the database.
    fs::path header_path = path_to_database / "header.json";
    std::ofstream header(header_path);
    if (header.is_open()) {
        json json_header;
        json_header["collections"] = collection_names;
        header << json_header << std::endl;
        header.close();
    }
    else {
        throw_failed_to_create_header("");
        return;
    }
}

void database::build_from_existing() {
    // Checks if the database exists.
    if (int ret = check_path_existence(this->path) != 0) return;

    // Read the header file.
    fs::path header = fs::path(this->path) / "header.json";
    json header_json = read_and_parse_json(header);
    std::vector<std::string> collection_names = header_json["collections"];

    for (const std::string &collection_name : collection_names) {
        fs::path collection_path = fs::path(this->path) / collection_name;
        collection* col = new collection(collection_path.string());
        this->set_collection(collection_name, col);
    }
}

std::string database::get_path() const {
    return this->path;
}

collection* database::get_collection(const std::string &col) const {
    auto itr = this->collections.find(col);
    if (itr == this->collections.end()) {
        throw_no_such_collection(col);
        return nullptr;
    }
    return this->collections.at(col);
}

void database::set_collection(const std::string &colletion_name, collection* col) {
    this->collections[colletion_name] = col;
}

void database::create_hash_index(const std::string &col_name, const std::vector<std::string> &field) {
    collection *col = this->get_collection(col_name);

    col->create_hash_index(field);
}

int database::create_document(const std::string &col_name, json &document) {
    return this->get_collection(col_name)->create_document(document);
}

std::vector<json> database::read(const std::string &col_name, const std::vector<condition_type> &conditions) {
    if (conditions.empty()) {
        // TODO: Read all documents.
        return {};
    }
    else return this->get_collection(col_name)->read_with_conditions(conditions);
}

void database::delete_collection(const std::string &col_name) {
    collection *col = this->get_collection(col_name);
    col->delete_collection();
    this->collections.erase(col_name);
    delete col;
}
