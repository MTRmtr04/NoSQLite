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


hash_index::hash_index(const std::string &path, const std::vector<std::string> &fields) : path(path) {
    this->build_index(fields);
}

std::string hash_index::get_path() {
    return this->path;
}

std::string hash_index::get_field() {
    return get_last_dir(this->path);
}

void hash_index::build_index(const std::vector<std::string> &fields) {

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
        if (documents.empty()) {
            // TODO: Error handling for failed paths/documents.
            continue;
        }

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
                index[hash].push_back(p.string());

                std::ofstream file(path_to_index_file);
                if (file.is_open()) file << index;
                else throw_failed_to_open_file(path_to_index_file);

                file.close();
            }
            else {
                std::ofstream file(path_to_index_file);
                if (file.is_open()) {
                    json index = {};
                    index[hash].push_back(p.string());
                    file << index;
                }
                else throw_failed_to_create_file(path_to_index_file);

                file.close();
            }
        }

    }

}