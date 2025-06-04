#include "collection.hpp"
#include "auxiliary.hpp"
#include <functional>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <json.hpp>
#include <omp.h>

using namespace nosqlite;
using json = nlohmann::json;

// TODO: Add index names to the header file so as to make creation at the start more efficient.

collection::collection(const std::string &path) : path(path), number_of_documents(0) {}

collection::~collection() {
    for (auto p : this->indexes) {
        delete p.second;
    }
}

int collection::build_from_scratch(const std::string &path_to_json) {

    // Delete everything from the directory where the collection will be stored if it exists, else create it.
    fs::path path_to_collection = this->path;
    if (fs::exists(path_to_collection))
        for (const fs::path &entry : fs::directory_iterator(path_to_collection))
            fs::remove_all(entry);
    else fs::create_directory(path_to_collection);

    if (path_to_json == "") {
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
            fs::remove_all(path_to_collection);
            return 1;
        }
        return 0;
    }
    
    // Checks if the directory with the json files to create the database with exists.
    if (int ret = check_path_existence(path_to_json) != 0) return 1;

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
        fs::remove_all(path_to_collection);
        return 1;
    }

    return 0;
}

int collection::build_from_existing() {
    // Checks if the collection exists.
    if (check_path_existence(this->path) != 0) return 1;
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

    return 0;
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

std::vector<json> collection::read(const field_type &field, const json &value) const {

    if (field.size() == 0) return {};

    std::vector<json> results;
    fs::path collection_path = this->path;

    std::string index_name = build_index_name(field);


    // Uses parallel processing to speed up the query.
    // Each thread has its own vector with results for the documents it collects.
    // Each thread's result vector gets added to the all_thread_results vector.
    // At the end of the parallel proccessing section all of the result in all_thread_results are pooled into the same vector and returned.
    std::vector<std::vector<json>> all_thread_results;
    int num_threads = 1;

    // Check if there is an index for the target field.
    if (this->indexes.find(index_name) != this->indexes.end()) {
        // Uses the index to perform the query.
        std::vector<std::string> paths = this->consult_hash_index(index_name, value);

        #pragma omp parallel
        {
            std::vector<json> thread_results;

            #pragma omp single
            {
                num_threads = omp_get_num_threads();
                all_thread_results.resize(num_threads);
            }

            #pragma omp barrier

            int thread_num = omp_get_thread_num();

            #pragma omp for
            for (int i = 0; i < paths.size(); i++) {
                const std::string &path = paths[i];

                json documents = read_and_parse_json(fs::path(path));
                for (const json &document : documents)
                    if (access_nested_fields(document, field) == value) thread_results.push_back(document);
            }

            all_thread_results[thread_num] = std::move(thread_results);
        }

        pool_results(all_thread_results, results);
        
    } else {
        // No index was found. Iterate through all the documents in the database.
        std::vector<fs::path> file_paths = {};
        collect_paths(collection_path, file_paths);

        #pragma omp parallel
        {
            std::vector<json> thread_results;

            #pragma omp single
            {
                num_threads = omp_get_num_threads();
                all_thread_results.resize(num_threads);
            }

            #pragma omp barrier

            int thread_num = omp_get_thread_num();

            #pragma omp for
            for (int i = 0; i < file_paths.size(); i++) {
                const fs::path &file_path = file_paths[i];

                json file_content = read_and_parse_json(file_path);
                
                for (const json &doc : file_content) {
                    json nested_value = access_nested_fields(doc, field);
                    if (nested_value == value) {
                        thread_results.push_back(doc);
                    }
                }
            }

            all_thread_results[thread_num] = std::move(thread_results);
        }

        pool_results(all_thread_results, results);
    }

    return results;
}

std::vector<json> collection::read_with_conditions(const std::vector<condition_type> &conditions) const {
    std::vector<json> results;
    fs::path collection_path = this->path;

    condition_type index_condition = {};
    std::string index_name = "";
    bool use_index = false;
    for(const auto &[field_path, op, target_value] : conditions){
        if (op == "==") {
            index_name = build_index_name(field_path);
            
            if (this->indexes.find(index_name) != this->indexes.end()) {
                index_condition = {field_path, op, target_value};
                use_index = true;
                break;
            }
        }
    }

    // Uses parallel processing to speed up the query.
    // Each thread has its own vector with results for the documents it collects.
    // Each thread's result vector gets added to the all_thread_results vector.
    // At the end of the parallel proccessing section all of the result in all_thread_results are pooled into the same vector and returned.
    std::vector<std::vector<json>> all_thread_results;
    int num_threads = 1;

    if (use_index) {
        std::vector<std::string> paths = this->consult_hash_index(index_name, index_condition.value);

        #pragma omp parallel
        {
            std::vector<json> thread_results;

            #pragma omp single
            {
                num_threads = omp_get_num_threads();
                all_thread_results.resize(num_threads);
            }

            #pragma omp barrier

            int thread_num = omp_get_thread_num();

            #pragma omp for
            for (int i = 0; i < paths.size(); i++) {
                const std::string &path = paths[i];

                json documents = read_and_parse_json(fs::path(path));
                for (const json &document : documents) {
                    bool satisfies_all = true;
                    for (const auto &[field_path, op, target_value] : conditions) {
                        try{
                            json actual_value = access_nested_fields(document, field_path);
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
                        thread_results.push_back(document);
                    }
                }
            }
            all_thread_results[thread_num] = std::move(thread_results);
        }
        pool_results(all_thread_results, results);

    } else {

        std::vector<fs::path> file_paths = {};
        collect_paths(collection_path, file_paths);

        #pragma omp parallel
        {
            std::vector<json> thread_results;

            #pragma omp single
            {
                num_threads = omp_get_num_threads();
                all_thread_results.resize(num_threads);
            }

            #pragma omp barrier

            int thread_num = omp_get_thread_num();

            #pragma omp for
            for (int i = 0; i < file_paths.size(); i++) {
                const fs::path &file_path = file_paths[i];
                
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
                        thread_results.push_back(doc);
                    }
                }
            }
            all_thread_results[thread_num] = std::move(thread_results);
        }
        pool_results(all_thread_results, results);
    }
    return results;
}

void collection::create_hash_index(const field_type &field) {

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
        std::cerr << "Error: Document with ID \"" << id << "\" does not exist." << std::endl;
        return 1;
    }

    json doc_array = read_and_parse_json(path_to_doc);
    bool updated = false;
    for(auto &doc : doc_array){
        if(doc.contains("id") && doc["id"] == id){
            field_type fields;
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
        std::cerr << "Error: Document with ID \"" << id << "\" not found in file." << std::endl;
        return 1;
    }
}

json collection::get_document(unsigned long long id) const {
    std::string idHash = hash_integer(id);
    fs::path path_to_doc = fs::path(this->path) / idHash.substr(0, 2) / idHash.substr(2, 2) / idHash.substr(4).append(".json");

    if (!fs::exists(path_to_doc)) {
        std::cerr << "Error: Document with ID \"" << id << "\" does not exist." << std::endl;
        return json();
    }

    json doc_array = read_and_parse_json(path_to_doc);
    for (const auto &doc : doc_array) {
        if (doc.contains("id") && doc["id"] == id) {
            return doc;
        }
    }

    std::cerr << "Error: Document with ID \"" << id << "\" not found inside file." << std::endl;
    return json();
}

int collection::delete_single(const fs::path &file_path, const std::vector<condition_type> &conditions) {
    int docs_removed = 0;
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

    return docs_removed;
}

int collection::delete_document(const field_type &field, const json &value) {
    return delete_with_conditions({{field, "==", value}});
}

int collection::delete_with_conditions(const std::vector<condition_type> &conditions) {
    int docs_removed = 0;
    fs::path collection_path = this->path;
    bool modified = false;

    condition_type index_condition = {};
    std::string index_name = "";
    bool use_index = false;
    for(const auto &[field_path, op, target_value] : conditions){
        if (op == "==") {
            index_name = build_index_name(field_path);
        
            if (this->indexes.find(index_name) != this->indexes.end()) {
                index_condition = {field_path, op, target_value};
                use_index = true;
                break;
            }
        }
    }

    // Uses parallel processing to speed up the query.
    // The docs_removed variable is shared among the threads.
    if (use_index) {
        std::vector<std::string> paths = consult_hash_index(index_name, index_condition.value);

        #pragma omp parallel for reduction(+ : docs_removed)
        for (int i = 0; i < paths.size(); i++) {
            const std::string &path = paths[i];

            docs_removed += delete_single(fs::path(path), conditions);
        }

    } else {
        std::vector<fs::path> file_paths = {};
        collect_paths(collection_path, file_paths);

        #pragma omp parallel for reduction(+ : docs_removed)
        for (int i = 0; i < file_paths.size(); i++) {
            fs::path file_path = file_paths[i];
            docs_removed += delete_single(file_path, conditions);
        }
    }

    // Update if documents were deleted
    if (docs_removed > 0) {
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

void collection::delete_collection() {
    fs::remove_all(fs::path(this->path));
}

bool collection::find_index(const field_type &field) {
    return this->indexes.find(build_index_name(field)) != this->indexes.end();
}

int collection::delete_hash_index(const field_type &field) {
    std::string index_name = build_index_name(field);
    if (!this->find_index(field)) {
        std::cerr << "Error: The index with name \"" << index_name << "\" does not exist." << std::endl;
        return 1;
    }
    hash_index* index = this->indexes[index_name];
    index->delete_index();
    delete index;
    this->indexes.erase(index_name);

    // TODO: Update header.
    
    return 0;
}
