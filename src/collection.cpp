#include "collection.hpp"
#include "auxiliary.hpp"
#include <functional>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <json.hpp>

using namespace nosqlite;
using json = nlohmann::json;

// TODO: Add index names to the header file so as to make creation at the start more efficient.

collection::collection(const std::string &path) : path(path) {
    this->build_from_existing();
}

collection::collection(const std::string &path, const std::string &path_to_json) : path(path), number_of_documents(0), indexes({}) {
    this->build_from_scratch(path_to_json);
}

collection::~collection() {
    // TODO: Everything necessary.
    for (auto p : this->indexes) {
        delete p.second;
    }
}

void collection::build_from_scratch(const std::string &path_to_json) {
    
    // Checks if the directory with the json files to create the database with exists.
    if (int ret = check_path_existence(path_to_json) != 0) return;

    // Delete everything from the directory where the collection will be stored if it exists, else create it.
    fs::path path_to_collection = this->path;
    if (fs::exists(path_to_collection))
        for (const fs::path &entry : fs::directory_iterator(path_to_collection))
            fs::remove_all(entry);
    else fs::create_directory(path_to_collection);

    // Get all the file JSON paths in the path_to_json directory
    std::vector<fs::path> paths;
    for (const fs::path &p : fs::recursive_directory_iterator(path_to_json)) {
        if (fs::is_directory(p) || p.extension() != ".json") continue;
        paths.push_back(p);
    }

    std::vector<fs::path> failed_paths;
    // Read all the json objects in the files and create a document for them in the database.
    for (const fs::path &p : paths) {
        json objects = read_and_parse_json(p);
        if (objects.empty()) {
            failed_paths.push_back(p);
            continue;
        }

        // In case the file has multiple objects in an array.
        if (objects.is_array()) {
            for (json object : objects) {
                if (int ret = this->add_document(object, false) != 0) {
                    failed_paths.push_back(p);
                    continue;
                }
            }
        }
        else {
            if (int ret = this->add_document(objects, false) != 0) {
                failed_paths.push_back(p);
                continue;
            }
        }
    }

    // Output error for the failed paths
    throw_failed_to_create_collection_entries(failed_paths);

    // Creates the header file for the database.
    fs::path header_path = fs::path(this->path) / "header.json";
    std::ofstream header(header_path);
    if (header.is_open()) {
        json json_header;
        json_header["number_of_documents"] = this->number_of_documents;
        header << json_header << std::endl;
        header.close();
    }
    else {
        throw_failed_to_create_header(this->get_name());
        return;
        // TODO: Roll back changes if header creation fails. CHECK WITH PROF
    }

    // TODO: Build collection's indices.
}

void collection::build_from_existing() {
    // Checks if the collection exists.
    if (int ret = check_path_existence(this->path) != 0) return;

    // Read the header file.
    fs::path header = fs::path(this->path) / "header.json";
    json header_json = read_and_parse_json(header);
    this->number_of_documents = header_json["number_of_documents"];

    // Build indices.
    fs::path indexes_path = fs::path(this->path) / "indexes";
    if (fs::exists(indexes_path)) {
        for (const fs::path &index_path : fs::directory_iterator(indexes_path)) {
            if (fs::is_directory(index_path)) {
                this->indexes[get_last_dir(index_path)] = new hash_index(index_path);
            }
        }
    }
}

std::string collection::get_path() const {
    return this->path;
}

std::string collection::get_name() const {
    return get_last_dir(this->path);
}

unsigned long long collection::get_number_of_documents() const {
    return this->number_of_documents;
}

int collection::add_document(json &json_object, bool update_header) {

    int ret = 0;

    json_object["id"] = this->number_of_documents;

    // TODO: Update indices

    // Hash the id.
    std::string idHash = hash_integer(this->number_of_documents);
    
    // Create the directory
    fs::path directory = fs::path(this->path) / idHash.substr(0, 2) / idHash.substr(2, 2);
    fs::create_directories(directory);

    // Build the path for the file according to the id hash.
    fs::path path_to_document = directory / idHash.substr(4).append(".json");
    
    // Prints the path for the created files
    //std::cout << "New document created here: " << path_to_document << std::endl;
    
    // Create a new one or edit the file if it already exists (in case of a hash collision).
    if (fs::exists(path_to_document)) {
        json json_file = read_and_parse_json(path_to_document);
        std::ofstream file(path_to_document);
        if (file.is_open()) {
            if (json_file.empty()) {
                ret = 1;
                update_header = false;
            }
            else {
                json_file.push_back(json_object);
                file << json_file << std::endl;
            }
        }
        else {
            throw_failed_to_open_file(path_to_document);
            ret = 1;
            update_header = false;
        }
        file.close();
    }
    else {
        std::ofstream file(path_to_document);
        if (file.is_open()) {
            file << '[' << json_object << ']' << std::endl;
        }
        else {
            throw_failed_to_create_file(path_to_document);
            ret = 1;
            update_header = false;
        }
        file.close();
    }


    // Once everything else is successful increment the number of documents.
    if (update_header) {
        std::fstream header(fs::path(this->path) / "header.json");
        if (header.is_open()) {
            json h;
            header >> h;
            h["number_of_documents"] = this->number_of_documents + 1;
            header << h;
            header.close();
            this->number_of_documents++;
        }
        else {
            throw_failed_to_open_file(fs::path(this->path) / "header.json");
            throw_failed_to_update_header(this->get_name());
            ret = 1;
            // TODO: Roll back changes if header update fails. CHECK WITH PROF
        }
    }
    else  this->number_of_documents++;

    return ret;
}

int collection::add_document(const std::string &json_content, bool update_header) {
    json document = read_and_parse_json(json_content);
    if (document.empty()) return 1;
    return this->add_document(document, update_header);
}

int collection::add_document(const std::string &json_content) {
    return this->add_document(json_content, true);
}
    
int nosqlite::collection::create_document(const json &new_document) {
    json doc_copy = new_document;

    // Valida se é realmente um objeto JSON
    if (!doc_copy.is_object()) {
        std::cerr << "Error: create_document received invalid JSON object." << std::endl;
        return 1;
    }

    return this->add_document(doc_copy, true);
}

std::vector<json> collection::read(const std::vector<std::string> &field, const json &value) const {

    if (field.size() == 0) return {};

    std::vector<json> results;
    fs::path collection_path = this->path;

    std::string index_name = build_index_name(field);

    if (this->indexes.find(index_name) != this->indexes.end()) {

        std::vector<std::string> paths = this->consult_hash_index(index_name, value);
        for (const std::string &path : paths) {

            json documents = read_and_parse_json(fs::path(path));
            for (const json &document : documents)
                if (access_nested_fields(document, field) == value) results.push_back(document);

        }
        
    } else {
        for (const fs::path &file_path : fs::recursive_directory_iterator(collection_path)) {
            if (file_path.extension() != ".json" || file_path.filename() == "header.json" || file_path.filename() == "index.json"){
                continue;
            } 

            json file_content = read_and_parse_json(file_path);
            for (const json &doc : file_content) {
                json nested_value = access_nested_fields(doc, field);
                if (nested_value == value) {
                    results.push_back(doc);
                }
            }
        }
    }
    return results;
}

std::vector<json> collection::read_with_conditions(const std::vector<std::tuple<std::vector<std::string>, std::string, json>> &conditions) const{
    std::vector<json> results;
    fs::path collection_path = this->path;

    std::function<bool(const json&, const std::string&, const json&)> compare;
    compare = [&](const json &doc_value, const std::string &op, const json &target) -> bool {
        if (doc_value.is_array()) {
            for (const auto &val : doc_value) {
                if (compare(val, op, target)) return true;
            }
            return false;
        }
    
        if (op == "==") return doc_value == target;
        if (op == "!=") return doc_value != target;
        if (op == ">")  return doc_value.is_number() && target.is_number() && doc_value > target;
        if (op == "<")  return doc_value.is_number() && target.is_number() && doc_value < target;
        if (op == ">=") return doc_value.is_number() && target.is_number() && doc_value >= target;
        if (op == "<=") return doc_value.is_number() && target.is_number() && doc_value <= target;
    
        return false;
    };

    for(const fs::path &file_path : fs::recursive_directory_iterator(collection_path)){
        if(file_path.extension() != ".json" || file_path.filename() == "header.json" || file_path.filename() == "index.json"){
            continue;
        }
        
        json file_content = read_and_parse_json(file_path);
        for(const json &doc : file_content){
            bool satisfies_all = true;

            for(const auto &[field_path, op, target_value] : conditions){
                try{
                    json actual_value = access_nested_fields(doc, field_path);
                    if(!compare(actual_value, op, target_value)){
                        satisfies_all = false;
                        break;
                    }
                } catch(...){
                    satisfies_all = false;
                    break;
                }
            }

            if(satisfies_all){
                results.push_back(doc);
            }
        }
    }
    return results;
}

void collection::create_hash_index(const std::vector<std::string> &field) {

    if (field.size() == 0) return;

    std::string name = build_index_name(field);

    if (this->indexes.find(name) != this->indexes.end()) return;

    std::string path = this->path + "/indexes/" + name;
    
    hash_index* index = new hash_index(path, field);

    this->indexes[name] = index;
}

std::vector<std::string> collection::consult_hash_index(const std::string &index_name, const json &value) const {
    hash_index *index = this->indexes.at(index_name);

    return index->consult(value);
}


//Update the entire object
int collection::update_document(unsigned long long id, const json& updated_data){
    std::string idHash = hash_integer(id);
    fs::path directory = fs::path(this->path) / idHash.substr(0, 2) / idHash.substr(2, 2);
    fs::path path_to_doc = directory / idHash.substr(4).append(".json");

    if(!fs::exists(path_to_doc)){
        std::cerr << "Error: Document with ID " << id << "does not exist." << std::endl;
        return 1;
    }

    json doc_array = read_and_parse_json(path_to_doc);
    bool updated = false;
    for(auto &doc : doc_array){
        if(doc.contains("id") && doc["id"] == id){
            std::vector<std::string> fields;
            for(auto it = updated_data.begin(); it != updated_data.end(); ++it){
                if(!doc.contains(it.key()) || doc[it.key()].is_null()){
                    fields.push_back(it.key());
                }
            }

            if(!fields.empty()){
                std::cerr << "Error: The following fields do not exist on the document you're accessing:\n";
                for(const auto& field : fields){
                    std::cerr << " - " << field << std::endl;
                }
                return 1;
            }

            for(auto it = updated_data.begin(); it != updated_data.end(); ++it){
                if(it.key() != "id"){
                    doc[it.key()] = it.value();
                }
            }

            updated = true;
            break;
        }
    }

    if(updated){
        std::ofstream file(path_to_doc);
        if(file.is_open()){
            file << doc_array.dump(4);
            file.close();
            return 0;
        } else {
            std::cerr << "Error: Failed to write updated document to file." << std::endl;
            return 1;
        }
    } else {
        std::cerr << "Error: Document with ID " << id << " not found in file." << std::endl;
        return 1;
    }
}

json collection::get_document(unsigned long long id) const {
    std::string idHash = hash_integer(id);
    fs::path path_to_doc = fs::path(this->path) / idHash.substr(0, 2) / idHash.substr(2, 2) / idHash.substr(4).append(".json");

    if (!fs::exists(path_to_doc)) {
        std::cerr << "Error: Document with ID " << id << " does not exist." << std::endl;
        return json();
    }

    json doc_array = read_and_parse_json(path_to_doc);
    for (const auto &doc : doc_array) {
        if (doc.contains("id") && doc["id"] == id) {
            return doc;
        }
    }

    std::cerr << "Error: Document with ID " << id << " not found inside file." << std::endl;
    return json();
}

int collection::delete_document(const std::string &field, const json &value) {
    std::vector<std::string> fields;
    std::stringstream ss(field);
    std::string segment;
    while (std::getline(ss, segment, '.')){
        fields.push_back(segment);
    } 

    return delete_with_conditions({{fields, "==", value}});
}

int collection::delete_with_conditions(const std::vector<std::tuple<std::vector<std::string>, std::string, json>> &conditions) {
    int docs_removed = 0;
    fs::path collection_path = this->path;
    bool modified = false;

    std::function<bool(const json&, const std::string&, const json&)> compare;
    compare = [&](const json &doc_value, const std::string &op, const json &target) -> bool {
        if (doc_value.is_array()) {
            for (const auto &val : doc_value) {
                if (compare(val, op, target)) return true;
            }
            return false;
        }
    
        if (op == "==") return doc_value == target;
        if (op == "!=") return doc_value != target;
        if (op == ">")  return doc_value.is_number() && target.is_number() && doc_value > target;
        if (op == "<")  return doc_value.is_number() && target.is_number() && doc_value < target;
        if (op == ">=") return doc_value.is_number() && target.is_number() && doc_value >= target;
        if (op == "<=") return doc_value.is_number() && target.is_number() && doc_value <= target;
    
        return false;
    };

    for (const fs::path &file_path : fs::recursive_directory_iterator(collection_path)) {
        if (file_path.extension() != ".json" || file_path.filename() == "header.json" || file_path.filename() == "index.json"){
            continue;
        } 

        json file_content = read_and_parse_json(file_path);
        json remaining_docs = json::array();
        bool file_modified = false;

        for (const auto &doc : file_content) {
            bool satisfies_all = true;

            for (const auto &[field_path, op, target_value] : conditions) {
                try {
                    json actual_value = access_nested_fields(doc, field_path);
                    if (!compare(actual_value, op, target_value)) {
                        satisfies_all = false;
                        break;
                    }
                } catch (...) {
                    satisfies_all = false;
                    break;
                }
            }

            if (satisfies_all) {
                docs_removed++;
                file_modified = true;
                modified = true;
            } else {
                remaining_docs.push_back(doc);
            }
        }

        // If documents were removed update the file
        if (file_modified) {
            if (remaining_docs.empty()) {
                fs::remove(file_path);
            } else {
                std::ofstream file(file_path);
                if (file.is_open()) {
                    file << remaining_docs << std::endl;
                    file.close();
                } else {
                    throw_failed_to_open_file(file_path);
                    return -1;
                }
            }
        }
    }

    // Update if documents were deleted
    if (modified) {
        fs::path header_path = fs::path(this->path) / "header.json";
        std::ofstream header(header_path);
        if (header.is_open()) {
            json json_header;
            json_header["number_of_documents"] = this->number_of_documents - docs_removed;
            header << json_header << std::endl;
            header.close();
            this->number_of_documents -= docs_removed;
        } else {
            throw_failed_to_open_file(header_path);
            throw_failed_to_update_header(this->get_name());
            return -1;
        }
    }

    return docs_removed;
}

