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

collection::collection(const std::string &path) : path(path), number_of_documents(0), parallel_processing(true) {}

collection::~collection() {
    for (auto p : this->indexes) {
        delete p.second;
    }
}

void collection::turn_on_parallel_processing() {
    this->parallel_processing = true;
}

void collection::turn_off_parallel_processing() {
    this->parallel_processing = false;
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
        std::ofstream header(fs::path(this->path) / "header.json");
        if (header.is_open()) {
            json h = {};
            h["number_of_documents"] = this->number_of_documents + 1;
            // header >> h;
            // h["number_of_documents"] = this->number_of_documents + 1;
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

std::vector<json> nosqlite::collection::read_all() const {
    std::vector<json> results;
    fs::path collection_path = this->path;

    // Uses parallel processing to speed up the query.
    // Each thread has its own vector with results for the documents it collects.
    // Each thread's result vector gets added to the all_thread_results vector.
    // At the end of the parallel processing section all of the result in all_thread_results are pooled into the same vector and returned.
    std::vector<std::vector<json>> all_thread_results;
    int num_threads = 1;

    std::vector<fs::path> file_paths = {};
    collect_paths(collection_path, file_paths);

    #pragma omp parallel if(this->parallel_processing)
    {
        std::vector<json> thread_results;

        #pragma omp single
        {
            #ifdef _OPENMP
                num_threads = omp_get_num_threads();
            #else
                num_threads = 1;
            #endif
            all_thread_results.resize(num_threads);
        }

        #pragma omp barrier

        #ifdef _OPENMP
            int thread_num = omp_get_thread_num();
        #else
            int thread_num = 0;
        #endif

        #pragma omp for
        for (int i = 0; i < file_paths.size(); i++) {
            const fs::path &file_path = file_paths[i];

            json file_content = read_and_parse_json(file_path);
            for (const json &doc : file_content) {
                thread_results.push_back(doc);
            }
        }

        all_thread_results[thread_num] = std::move(thread_results);
    }

    pool_results(all_thread_results, results);

    return results;
}

std::vector<json> collection::read_with_conditions(const std::vector<condition_type> &conditions) const {
    std::vector<json> results;
    fs::path collection_path = this->path;
    field_type id = {"id"};
    for(const auto &[field_path, op, target_value] : conditions) {
        if (op == "==")
            // Read by document id
            if (field_path == id) return { this->get_document(target_value) };
    }

    condition_type index_condition = {};
    std::string index_name = "";
    bool use_index = false;
    for(const auto &[field_path, op, target_value] : conditions) {
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
    // At the end of the parallel processing section all of the result in all_thread_results are pooled into the same vector and returned.
    std::vector<std::vector<json>> all_thread_results;
    int num_threads = 1;

    if (use_index) {
        std::vector<std::string> paths = this->consult_hash_index(index_name, index_condition.value);

        #pragma omp parallel if(this->parallel_processing)
        {
            std::vector<json> thread_results;

            #pragma omp single
            {
                #ifdef _OPENMP
                    num_threads = omp_get_num_threads();
                #else
                    num_threads = 1;
                #endif
                all_thread_results.resize(num_threads);
            }

            #pragma omp barrier

            #ifdef _OPENMP
                int thread_num = omp_get_thread_num();
            #else
                int thread_num = 0;
            #endif

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

        #pragma omp parallel if(this->parallel_processing)
        {
            std::vector<json> thread_results;

            #pragma omp single
            {
                #ifdef _OPENMP
                    num_threads = omp_get_num_threads();
                #else
                    num_threads = 1;
                #endif
                all_thread_results.resize(num_threads);
            }

            #pragma omp barrier

            #ifdef _OPENMP
                int thread_num = omp_get_thread_num();
            #else
                int thread_num = 0;
            #endif

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

int collection::create_hash_index(const field_type &field) {

    if (field.size() == 0) return 0;

    std::string name = build_index_name(field);

    if (this->indexes.find(name) != this->indexes.end()) {
        std::cerr << "Index with name \"" << name << "\" already exists" << std::endl;
        return 1;
    }

    std::string path = this->path + "/indexes/" + name;
    
    hash_index* index = new hash_index(path);
    if (index->build_index(field) != 0) {
        std::cerr << "Failed to create hash index: \"" << name << "\"" << std::endl;
        return 1;
    }

    this->indexes[name] = index;
    return 0;
}

std::vector<std::string> collection::consult_hash_index(const std::string &index_name, const json &value) const {
    hash_index *index = this->indexes.at(index_name);

    return index->consult(value);
}

int collection::update_document(unsigned long long id, const json& updated_data, json &final) {
    if (updated_data.empty()) {
        std::cerr << "Error: No data provided to update." << std::endl;
        return 1;
    }

    std::string idHash = hash_integer(id);
    fs::path directory = fs::path(this->path) / idHash.substr(0, 2) / idHash.substr(2, 2);
    fs::path path_to_doc = directory / idHash.substr(4).append(".json");

    if(!fs::exists(path_to_doc)){
        std::cerr << "Error: Document with ID \"" << id << "\" does not exist." << std::endl;
        return 1;
    }

    json doc_array = read_and_parse_json(path_to_doc);
    json original;
    bool updated = false;
    for(auto &doc : doc_array){
        if(doc.contains("id") && doc["id"] == id){
            original = json(doc);
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

            final = doc;
            updated = true;
            break;
        }
    }

    if(updated){
        std::ofstream file(path_to_doc);
        if(file.is_open()){
            file << doc_array.dump(4);
            file.close();
            std::vector<std::string> possible_indices = build_possible_index_names(updated_data);
            for (const auto &index_name : possible_indices) {
                auto hsh_idx_it = this->indexes.find(index_name);
                if (hsh_idx_it == this->indexes.end()) continue;
                hsh_idx_it->second->update_index(original, final, path_to_doc.string());
            }
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

std::vector<json> nosqlite::collection::update_document(const std::vector<condition_type> &conditions, const json &updated_data) {
    if (updated_data.empty()) {
        std::cerr << "Error: No data provided to update." << std::endl;
        return {};
    }

    std::vector<json> matching_docs;
    // Get all documents that match the conditions.
    if (conditions.empty()) {
        // std::cerr << "Error: No conditions provided for update." << std::endl;
        // return {};
        matching_docs = this->read_all();
    }
    else matching_docs = this->read_with_conditions(conditions);
    
    if (matching_docs.empty()) {
        std::cerr << "Error: No documents match the provided conditions." << std::endl;
        return {};
    }
    
    std::vector<json> updated;

    #pragma omp parallel for if(this->parallel_processing)
    for (const json &doc : matching_docs) {
        unsigned long long id = doc["id"];
        json j;
        if (this->update_document(id, updated_data, j) == 0) {
            #pragma omp critical
            {
                // TODO: Should return updated document. This one is the original.
                updated.push_back(j);
            }
        }
        else {
            #pragma omp critical
            {
                std::cerr << "Error: Failed to update document with ID \"" << id << "\"." << std::endl;
                std::cerr << "Continuing..." << std::endl;
            }
        }
    }

    return updated;
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

        #pragma omp parallel for if(this->parallel_processing) reduction(+ : docs_removed)
        for (int i = 0; i < paths.size(); i++) {
            const std::string &path = paths[i];

            docs_removed += delete_single(fs::path(path), conditions);
        }

    } else {
        std::vector<fs::path> file_paths = {};
        collect_paths(collection_path, file_paths);

        #pragma omp parallel for if(this->parallel_processing) reduction(+ : docs_removed)
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
