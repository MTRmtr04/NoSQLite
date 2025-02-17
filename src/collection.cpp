#include "collection.hpp"

using namespace nosqlite;


collection::collection(const std::string &name) : name(name) {
    // TODO: Get the number of documents from the collection header.
}

collection::collection(const std::string &name, const std::string &path_to_json) : name(name) {
    this->build_from_scratch(path_to_json);
}


collection::~collection() {
    // TODO: Everything necessary.
}


void collection::build_from_scratch(const std::string &path_to_json) {
    // TODO: Build collection itself.
    // TODO: Build collection's header file.
    // TODO: Build collection's indices. 
}