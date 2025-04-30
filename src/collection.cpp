#include "collection.hpp"
#include "auxiliary.hpp"
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
    std::string id_hash = hash_integer(this->number_of_documents);
    
    // Create the directory
    fs::path directory = fs::path(this->path) / id_hash.substr(0, 2) / id_hash.substr(2, 2);
    fs::create_directories(directory);

    // Build the path for the file according to the id hash.
    fs::path path_to_document = directory / id_hash.substr(4).append(".json");
    
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


