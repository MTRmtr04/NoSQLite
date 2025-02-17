#include "database.hpp"

using namespace nosqilte;

    
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
    // TODO: Build database header file.
    // TODO: Build each collection's header file.
    // TODO: Build each collection's indices. 
}

std::string database::get_path() {
    return this->path;
}
