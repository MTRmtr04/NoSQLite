#include <iostream>
#include <filesystem>
#include <fstream>
#include "database.hpp"
#include "auxiliary.hpp"
#include "json.hpp"

using json = nlohmann::json;
using namespace nosqlite;
namespace fs = std::filesystem;

    
database::database(const std::string &path) : path(path) {
    // TODO: Build collection instances.
    // TODO: Build index instances.
    // TODO: Set any other attributes with info on the database.
}

database::database(const std::string &path_to_database, const std::string &path_to_json) : path(path_to_database) {
    this->build_from_scratch(path_to_json);
}

database::~database() {
    // TODO: Everything necessary.
}

void database::build_from_scratch(const std::string &path_to_json) {

    // Checks if the directory with the json files to create the database with exists.
    if (int ret = check_path_existence(path_to_json) != 0) {
        std::cerr << "    The directory with the files to create the database doesn't exist." << std::endl;
        return;
    }

    // Delete everything from the directory where the database will be stored if it exists, else create it.
    fs::path path_to_database = this->path;
    if (fs::exists(path_to_database))
        for (const fs::path &entry : fs::directory_iterator(path_to_database))
            fs::remove_all(entry);
    else fs::create_directory(path_to_database);

    
    std::vector<std::string> collection_names;
    // Create each individual collection
    for (const fs::path &entry : fs::directory_iterator(path_to_json)) {
        if (!fs::is_directory(entry)) continue;
        std::string collection_name = get_last_dir(entry.string());
        collection_names.push_back(collection_name);

        fs::path collection_path = fs::path(this->path) / collection_name;

        collection col(collection_path.string(), entry.parent_path().string());

        this->collections[collection_name] = &col;
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
        std::cerr << "Failed to create database header file." << std::endl;
        return;
    }
}

std::string database::get_path() const {
    return this->path;
}

collection* database::get_collection(const std::string &col) {
    auto itr = this->collections.find(col);
    if (itr == this->collections.end()) {
        std::cerr << "Collection: \"" << col << "\" does not exist in this database." << std::endl;
        return nullptr;
    }
    return this->collections.at(col);
}
