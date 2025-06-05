#include "hash_index.hpp"
#include "auxiliary.hpp"
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <filesystem>
#include <json.hpp>

using namespace nosqlite;
using json = nlohmann::json;
namespace fs = std::filesystem;


hash_index::hash_index(const std::string path) : path(path) {}

std::string hash_index::get_path() {
    return this->path;
}

std::string hash_index::get_field() {
    return get_last_dir(this->path);
}

int hash_index::build_index(const field_type &fields) {

    // Delete everything from the directory where the index will be stored if it exists, else create it.
    fs::path path_to_index(this->path);
    if (fs::exists(path_to_index))
        for (const fs::path &entry : fs::directory_iterator(path_to_index))
            fs::remove_all(entry);
    else fs::create_directories(path_to_index);

    fs::path col = path_to_index.parent_path().parent_path();

    // Get all the file JSON paths in the collection except the header.
    std::vector<fs::path> paths;
    for (const fs::path &p : fs::recursive_directory_iterator(col)) {
        if (fs::is_directory(p) || p.extension() != ".json" || p.filename() == "header.json") continue;
        paths.push_back(p);
    }

    for (const fs::path &p : paths) {
        json documents = read_and_parse_json(p);
        if (documents.empty()) continue;

        for (json document : documents) {
            json indexee = access_nested_fields(document, fields);
            std::string hash;

            if (indexee == nullptr) hash = hash_string("NULL");
            else hash = hash_json(indexee);

            // Create the directory
            fs::path directory = fs::path(this->path) / hash.substr(0, 2) / hash.substr(2, 2);
            fs::create_directories(directory);

            fs::path path_to_index_file = directory / "index.json";

            if (fs::exists(path_to_index_file)) {
                json index = read_and_parse_json(path_to_index_file);
                index[hash.substr(4)].push_back(p.string());

                std::ofstream file(path_to_index_file);
                if (file.is_open()) {
                    file << index;
                    file.close();
                }
                else {
                    throw_failed_to_open_file(path_to_index_file);
                    fs::remove_all(path_to_index);
                    return 1;
                }
            }
            else {
                std::ofstream file(path_to_index_file);
                if (file.is_open()) {
                    json index = {};
                    index[hash.substr(4)].push_back(p.string());
                    file << index;
                    file.close();
                }
                else {
                    throw_failed_to_create_file(path_to_index_file);
                    fs::remove_all(path_to_index);
                    return 1;
                }
            }
        }
    }

    return 0;
}

std::vector<std::string> hash_index::consult(const json &value) {
    
    std::vector<std::string> documents({});
    
    std::string hash = value == nullptr ? hash_string("NULL") : hash_json(value);

    fs::path index_file_path(this->path);
    index_file_path = index_file_path / hash.substr(0, 2) / hash.substr(2, 2) / "index.json";
    if (fs::exists(index_file_path)) {
        json index_file = read_and_parse_json(index_file_path);

        json doc = index_file[hash.substr(4)];

        if (doc != nullptr) documents = doc;
    }

    return documents;
}

void hash_index::update_index(json original_value, json updated_value, const std::string &document_path) {
    //Uses original_value to find and delete index entry for document in document_path
    std::string hash = hash_json(original_value);
    fs::path path_to_index_file = fs::path(this->path) / hash.substr(0, 2) / hash.substr(2, 2) / "index.json";;
    if (!fs::exists(path_to_index_file)) {
        //TODO: error handling
        return;
    }
    json index = read_and_parse_json(path_to_index_file);
    size_t n = index[hash.substr(4)].erase(document_path);

    if (n == 0) {
       //TODO: error handling 
       return;
    }

    std::ofstream file(path_to_index_file);
    if (file.is_open()) file << index;
    else throw_failed_to_open_file(path_to_index_file);

    file.close();

    //Created new index entry with updated_value
    hash = hash_json(updated_value);
    path_to_index_file = fs::path(this->path) / hash.substr(0, 2) / hash.substr(2, 2) / "index.json";
    if (fs::exists(path_to_index_file)) {
        json index = read_and_parse_json(path_to_index_file);
        index[hash.substr(4)].push_back(document_path);

        std::ofstream file(path_to_index_file);
        if (file.is_open()) file << index;
        else throw_failed_to_open_file(path_to_index_file);

        file.close();
    }
    else {
        std::ofstream file(path_to_index_file);
        if (file.is_open()) {
            json index = {};
            index[hash.substr(4)].push_back(document_path);
            file << index;
        }
        else throw_failed_to_create_file(path_to_index_file);

        file.close();
    }
}

void hash_index::delete_index() {
    fs::remove_all(this->get_path());
}
