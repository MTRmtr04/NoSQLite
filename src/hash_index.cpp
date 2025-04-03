#include "hash_index.hpp"
#include "auxiliary.hpp"
#include <fstream>
#include <iostream>
#include <json.hpp>

using namespace nosqlite;
using json = nlohmann::json;


hash_index::hash_index(const std::string &path) : path(path) {
    this->build_index();
}

std::string hash_index::get_path() {
    return this->path;
}

std::string hash_index::get_field() {
    return get_last_dir(this->path);
}

void hash_index::build_index() {}