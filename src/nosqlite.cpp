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
    if (this->db->build_from_existing() != 0) {
        delete this->db;
        exit(1);
    }
    this->clear_all();
}

nosqlite_api::nosqlite_api(const std::string &path_to_database, const std::string &path_to_json) {
    this->db = new database(path_to_database);
    if (this->db->build_from_scratch(path_to_json) != 0) {
        delete this->db;
        exit(1);
    }
    this->clear_all();
}

nosqlite_api::~nosqlite_api() {
    delete this->db;
}

void nosqlite_api::turn_on_parallel_processing() {
    this->db->turn_on_parallel_processing();
}

void nosqlite_api::turn_off_parallel_processing() {
    this->db->turn_off_parallel_processing();
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
            results = this->db->update(this->active_collection, this->conditions, this->active_json);
            ret = 0;
            break;
        }
        case REMOVE: {

            if (this->conditions.empty()) {
                std::cerr << "Error: No conditions provided for removal." << std::endl;
            } else {
                int i = this->db->remove(this->active_collection, this->conditions);
                results.assign(i, json::object());
            }
            ret = 0;
            break;
        }
        case DELETE_INDEX: {
            ret = this->db->delete_hash_index(this->active_collection, this->active_field);
            break;
        }
        case CREATE_INDEX: {
            ret = this->db->create_hash_index(this->active_collection, this->active_field);
            break;
        }
        case DELETE_COLLECTION: {
            ret = this->db->delete_collection(this->active_collection);
            break;
        }
        case CREATE_COLLECTION: {
            ret = this->db->create_collection(this->active_collection, this->active_path);
            break;
        }
        default: {
            std::cerr << "No query to execute" << std::endl;
        }
    }

    this->clear_all();

    return ret;
}

nosqlite_api* nosqlite_api::create(std::string col_name, json document) {
    this->active_query_type = CREATE;
    this->active_collection = col_name;
    this->active_json = document;
    return this;
}

nosqlite_api* nosqlite_api::read(std::string col_name, condition_type condition ) {
    this->active_query_type = READ;
    this->active_collection = col_name;
    if (valid_condition(condition)) this->conditions.push_back(condition);
    return this;
}

nosqlite_api *nosqlite::nosqlite_api::update(std::string col_name, json update_data, condition_type condition) {
    this->active_query_type = UPDATE;
    this->active_collection = col_name;
    this->active_json = update_data;

    if (valid_condition(condition)) {
        this->conditions.push_back(condition);
    }
    return this;
}

nosqlite_api *nosqlite::nosqlite_api::remove(std::string col_name, condition_type condition) {
    this->active_query_type = REMOVE;
    this->active_collection = col_name;
    
    if (valid_condition(condition)) {
        this->conditions.push_back(condition);
    }
    return this;
}

nosqlite_api *nosqlite_api::AND(condition_type condition)
{
  if (valid_condition(condition))
    this->conditions.push_back(condition);
  return this;
}

bool nosqlite_api::valid_condition(condition_type condition) {
    if (condition == empty_condition) return false;
    return condition.op == "==" || condition.op == "!=" || condition.op == ">" || condition.op == "<" || condition.op == ">=" || condition.op == "<=";
}

nosqlite_api* nosqlite_api::delete_collection(const std::string &col_name) {
    this->active_query_type = DELETE_COLLECTION;
    this->active_collection = col_name;
    return this;
}

nosqlite_api* nosqlite_api::create_collection(const std::string &col_name, const std::string &path_to_json) {
    this->active_query_type = CREATE_COLLECTION;
    this->active_collection = col_name;
    this->active_path = path_to_json;

    return this;
}

nosqlite_api* nosqlite_api::delete_index(const std::string &col_name, const field_type &field) {
    this->active_query_type = DELETE_INDEX;
    this->active_collection = col_name;
    this->active_field = field;
    return this;
}

nosqlite_api* nosqlite_api::create_index(const std::string &col_name, const field_type &field) {
    this->active_query_type = CREATE_INDEX;
    this->active_collection = col_name;
    this->active_field = field;
    return this;
}
