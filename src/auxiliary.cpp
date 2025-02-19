#include <iostream>
#include <fstream>
#include "auxiliary.hpp"


// TODO: Adapt for Windows file path.
std::string nosqlite::get_last_dir(std::string path) {
    std::stringstream ss(path);
    std::string t;
    while (getline(ss, t, '/'));
    return t;
}

int nosqlite::check_path_existence(const std::string &path) {
    fs::path fs_path = path;
    if (!fs::exists(fs_path)) {
        std::cerr << "Error: Path doesn't exist. No such file or directory: \"" << path << "\"" << std::endl;
        return 1;
    }
    return 0;
}

std::string nosqlite::hash_integer(unsigned long long num) {
    std::hash<std::string> hash_func;
    size_t hash = hash_func(std::to_string(num));

    std::stringstream stream;
    stream << std::hex << hash;
    return stream.str();
}

void nosqlite::throw_failed_to_open_file(fs::path path) {
    std::cerr << "Error: Failed to open file: " << path << "." << std::endl;
}

void nosqlite::throw_failed_to_create_file(fs::path path) {
    std::cerr << "Error: Failed to create file: " << path << "." << std::endl;
}

void nosqlite::throw_failed_to_create_header(std::string collection_name) {
    if (collection_name.empty())std::cerr << "Error: Failed to create database header file" << std::endl;
    else std::cerr << "Error: Failed to create collection header file. Collection: \"" << collection_name << "\"." << std::endl;
}

void nosqlite::throw_failed_to_update_header(std::string collection_name) {
    if (collection_name.empty())std::cerr << "Error: Failed to update database header file" << std::endl;
    else std::cerr << "Error: Failed to update collection header file. Collection: \"" << collection_name << "\"." << std::endl;
}

void nosqlite::throw_no_such_collection(std::string collection_name) {
    std::cerr << "Collection: \"" << collection_name << "\" does not exist in this database." << std::endl;
}

void nosqlite::throw_failed_to_create_collection_entries(const std::vector<fs::path> &failed_paths) {
    if(!failed_paths.empty()) {
        std::cerr << "Error: Failed to create database entries for the JSON objects at the following locations:" << std::endl;
        for (fs::path fail : failed_paths) {
            std::cerr << "    " << fail << std::endl;
        }
    }
}

json nosqlite::read_and_parse_json(fs::path path) {
    std::ifstream file(path);
    json object;
    if (file.is_open()) {
        try {
            object = json::parse(file);
        }
        catch(const nlohmann::json::parse_error& e) {
            std::cerr << "Error: Invalid JSON in the file at: " << path << "." << std::endl;
        }
    }
    else {
        throw_failed_to_open_file(path);
    }
    return object;   
}

json nosqlite::read_and_parse_json(std::string json_content) {
    json object;
    try {
        object = json::parse(json_content);
    }
    catch(const nlohmann::json::parse_error& e) {
        std::cerr << "Error: Invalid JSON string." << std::endl;
    }
    return object;   
}

