#include "collection.hpp"
#include "auxiliary.hpp"
#include <fstream>
#include <iostream>
#include <json.hpp>

using namespace nosqlite;
using json = nlohmann::json;


collection::collection(const std::string &path) : path(path) {
    this->build_from_existing();
}

collection::collection(const std::string &path, const std::string &path_to_json) : path(path), number_of_documents(0) {
    this->build_from_scratch(path_to_json);
}

collection::~collection() {
    // TODO: Everything necessary.
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

    // Get all the file paths in the path_to_json directory
    std::vector<fs::path> paths;
    for (fs::path p : fs::recursive_directory_iterator(path_to_json)) {
        if (fs::is_directory(p)) continue;
        paths.push_back(p);
    }

    std::vector<fs::path> failed_paths;
    // Read all the json objects in the files and create a document for them in the database.
    for (fs::path p : paths) {
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

    // TODO: Build indices.
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

    // Create a new one or edit the file if it already exists (in case of a hash collision).
    if (fs::exists(path_to_document)) {
        std::ofstream file(path_to_document);
        if (file.is_open()) {
            json json_file = read_and_parse_json(path_to_document);
            if (json_file.empty()) {
                ret = 1;
                update_header = false;
            }
            else {
                json_file.push_back(json_object);
                file << json_file << std::endl;
            }
            file.close();
        }
        else {
            throw_failed_to_open_file(path_to_document);
            ret = 1;
            update_header = false;
        }
    }
    else {
        std::ofstream file(path_to_document);
        if (file.is_open()) {
            file << '[' << json_object << ']' << std::endl;
            file.close();
        }
        else {
            throw_failed_to_create_file(path_to_document);
            ret = 1;
            update_header = false;
        }
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

json collection::get_document(unsigned long long id) const {
    // Hash the id to find the file path
    std::string id_hash = hash_integer(id);

    // Create the directory
    fs::path directory = fs::path(this->path) / id_hash.substr(0, 2) / id_hash.substr(2, 2);
    fs::path path_to_document = directory / id_hash.substr(4).append(".json");

    if (!fs::exists(path_to_document)) {
        std::cerr << "Error: Document with id " << id << " does not exist." << std::endl;
        return json();
    }

    // Read the JSON file
    json document = read_and_parse_json(path_to_document);
    if (document.empty()) {
        std::cerr << "Error: Document with id " << id << " is empty." << std::endl;
        return json();
    }

    // Find the document with the given id
    for (const auto& doc : document) {
        if (doc.contains("id") && doc["id"] == id) {
            return doc;
        }
    }
    return json();
}

std::vector<json> collection::get_all_documents() const {
    std::vector<json> documents;
    for (unsigned long long i=0; i < this->number_of_documents; i++) {
        json document = this->get_document(i);
        if (!document.empty()) {
            documents.push_back(document);
        }
    }
    return documents;
}

std::vector<json> collection::find_all() const {
    return get_all_documents();
}

std::vector<json> collection::find_by_title(const std::string &title) const {
    std::vector<json> results;
    std::vector<json> all_docs = get_all_documents();

    for (const auto& doc : all_docs) {
        if (doc.contains("title")) {
            std::string movie_title = doc["title"];
            if (movie_title == title) {
                results.push_back(doc);
            }
        }
    }

    return results;
}

std::vector<json> collection::find_by_genre(const std::string &genre) const {
    std::vector<json> results;
    std::vector<json> all_docs = get_all_documents();

    for (const auto& doc : all_docs) {
        if (doc.contains("genre")) {
            std::string movie_genre = doc["genre"];
            if (movie_genre == genre) {
                results.push_back(doc);
            }
        }
    }

    return results;
}

std::vector<json> collection::find_by_director(const std::string &director) const {
    std::vector<json> results;
    std::vector<json> all_docs = get_all_documents();

    for (const auto& doc : all_docs) {
        if (doc.contains("director")) {
            std::string movie_director = doc["director"];
            if (movie_director == director) {
                results.push_back(doc);
            }
        }
    }

    return results;
}

std::vector<json> collection::find_by_year(int year) const {
    std::vector<json> results;
    std::vector<json> all_docs = get_all_documents();

    for (const auto& doc : all_docs) {
        if (doc.contains("year") && doc["year"] == year) {
            results.push_back(doc);
        }
    }

    return results;
}