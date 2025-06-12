#include <iostream>
#include <filesystem>
#include <fstream>
#include "database.hpp"
#include "auxiliary.hpp"
#include <json.hpp>

using json = nlohmann::json;
using namespace nosqlite;
namespace fs = std::filesystem;

    
database::database(const std::string &path) : path(path) {}

database::~database() {
    for (auto p : this->collections) {
        delete p.second;
    }
}

void database::turn_on_parallel_processing() {
    for (auto col : this->collections) col.second->turn_on_parallel_processing();
}

void database::turn_off_parallel_processing() {
    for (auto col : this->collections) col.second->turn_off_parallel_processing();
}

int database::build_from_scratch(const std::string &path_to_json) {

    // Checks if the directory with the json files to create the database with exists.
    if (int ret = check_path_existence(path_to_json) != 0) return 1;

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

        collection* col = new collection(collection_path.string());
        if (col->build_from_scratch((entry.parent_path() / collection_name).string()) != 0) {
            std::cerr << "Error: Failed to create collection: \"" << collection_name << "\"" << std::endl;
            delete col;
        }
        else this->collections[collection_name] = col;
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
        return 1;
    }

    return 0;
}

int database::build_from_existing() {
    // Checks if the database exists.
    if (int ret = check_path_existence(this->path) != 0) return 1;

    // Read the header file.
    fs::path header = fs::path(this->path) / "header.json";
    json header_json = read_and_parse_json(header);
    std::vector<std::string> collection_names = header_json["collections"];

    std::vector<std::string> new_col_names = {};
    for (const std::string &collection_name : collection_names) {
        fs::path collection_path = fs::path(this->path) / collection_name;
        collection* col = new collection(collection_path.string());
        if (col->build_from_existing() != 0) {
            std::cerr << "Error: Failed to build collection: \"" << collection_name << "\"" << std::endl;
            delete col;
        }
        else {
            this->set_collection(collection_name, col);
            new_col_names.push_back(collection_name);
        }
    }

    std::ofstream header_file(header);
    if (header_file.is_open()) {
        json header_json;
        header_json["collections"] = new_col_names;
        header_file << header_json << std::endl;;
    } else {
        throw_failed_to_update_header("");
        return 1;
    }

    return 0;
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

int database::create_hash_index(const std::string &col_name, const field_type &field) {
    collection *col = this->get_collection(col_name);

    return col->create_hash_index(field);
}

int database::create_document(const std::string &col_name, json &document) {
    return this->get_collection(col_name)->create_document(document);
}

std::vector<json> database::read(const std::string &col_name, const std::vector<condition_type> &conditions) {
    if (conditions.empty()) {
        return this->get_collection(col_name)->read_all(); 
    }
    else return this->get_collection(col_name)->read_with_conditions(conditions);
}

std::vector<json> nosqlite::database::update(const std::string &col_name, const std::vector<condition_type> &conditions, const json &updated_data) {
    if (conditions.empty()) {
        std::cerr << "Error: No conditions provided for update." << std::endl;
        return {};
    }

    if (updated_data.empty()) {
        std::cerr << "Error: No data provided to update." << std::endl;
        return {};
    }

    return this->get_collection(col_name)->update_document(conditions, updated_data);
}

int database::delete_collection(const std::string &col_name) {
    if (this->collections.find(col_name) == this->collections.end()) {
        std::cerr << "Error: Collection with name \"" << col_name << "\" does not exist." << std::endl;
        return 1;
    }
    collection *col = this->get_collection(col_name);
    col->delete_collection();
    this->collections.erase(col_name);
    delete col;

    fs::path header_path = fs::path(this->path) / "header.json";
    json header = read_and_parse_json(header_path);
    std::vector<std::string> cols;
    header["collections"].get_to(cols);
    std::vector<std::string> new_cols = {};
    for (const std::string &col : cols) {
        if (col == col_name) continue;
        new_cols.push_back(col);
    }
    header["collections"] = new_cols;

    std::ofstream header_file(header_path);
    if (header_file.is_open()) {
        header_file << header << std::endl;
        header_file.close();
    }
    else {
        throw_failed_to_update_header("");
        return 1;
    }
    return 0;
}

int database::create_collection(const std::string &col_name, const std::string &path_to_files) {
    if (this->collections.find(col_name) != this->collections.end()) {
        std::cerr << "Error: Collection with name \"" << col_name << "\" already exists." << std::endl;
        return 1;
    }

    collection* col = new collection(this->path + "/" + col_name);
    if (col->build_from_scratch(path_to_files) != 0) {
        std::cerr << "Error: Failed to build collection: \"" << col_name << "\"" << std::endl;
        delete col;
        return 1;
    }
    else this->collections[col_name] = col;

    fs::path header_path = fs::path(this->path) / "header.json";
    json header = read_and_parse_json(header_path);   
    header["collections"].push_back(col_name);

    std::ofstream header_file(header_path);
    if (header_file.is_open()) {
        header_file << header << std::endl;
        header_file.close();
    }
    else {
        throw_failed_to_update_header("");
        return 1;
    }

    return 0;
}

int database::delete_hash_index(const std::string &col_name, const field_type &field) {
    if (this->collections.find(col_name) == this->collections.end()) {
        std::cerr << "Error: Collection with name \"" << col_name << "\" does not exist." << std::endl;
        return 1;
    }

    return this->get_collection(col_name)->delete_hash_index(field);
}
