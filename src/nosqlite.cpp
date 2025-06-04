#include "auxiliary.hpp"
#include "database.hpp"
#include "collection.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <json.hpp>
#include "nosqlite.hpp"

using namespace nosqlite;
using json = nlohmann::json;


nosqlite_api::nosqlite_api(const std::string &path) {
    this->db = new database(path);
    this->clear_all();
}

nosqlite_api::nosqlite_api(const std::string &path_to_database, const std::string &path_to_json) {
    this->db = new database(path_to_database, path_to_json);
    this->clear_all();
}

nosqlite_api::~nosqlite_api() {
    delete this->db;
}

void nosqlite_api::clear_all() {
    this->active_collection = "";
    this->active_field = {};
    this->active_json = {};
    this->active_query_type = NONE;
    this->conditions = {};
}

int nosqlite_api::execute(std::vector<json> &results) {
    int ret = 1;

    switch (this->active_query_type) {
        case CREATE: {
            ret = this->db->create_document(this->active_collection, this->active_json);
            break;
        }
        case READ: {
            results = this->db->read(this->active_collection, this->conditions);
            ret = 0;
            break;
        }
        case UPDATE: {
            break;
        }
        case DELETE: {
            break;
        }
        case CREATE_INDEX: {
            break;
        }
        case DELETE_COLLECTION: {
            this->db->delete_collection(this->active_collection);
            ret = 0;
            break;
        }
        default: {
            std::cerr << "No query to execute" << std::endl;
        }
    }

    this->clear_all();

    return ret;
}

void nosqlite_api::create(std::string col_name, json document) {
    this->active_query_type = CREATE;
    this->active_collection = col_name;
    this->active_json = document;
}

nosqlite_api* nosqlite_api::read(std::string col_name, condition_type condition ) {
    this->active_query_type = READ;
    this->active_collection = col_name;
    if (valid_condition(condition)) this->conditions.push_back(condition);
    return this;
}

nosqlite_api* nosqlite_api::AND(condition_type condition) {
    if (valid_condition(condition)) this->conditions.push_back(condition);
    return this;
}

bool nosqlite_api::valid_condition(condition_type condition) {
    if (condition == empty_condition) return false;
    return condition.op == "==" || condition.op == "!=" || condition.op == ">" || condition.op == "<" || condition.op == ">=" || condition.op == "<=";
}

void nosqlite_api::delete_collection(const std::string &col_name) {
    this->active_query_type = DELETE_COLLECTION;
    this->active_collection = col_name;
}

