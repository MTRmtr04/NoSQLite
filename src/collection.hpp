/**
 * @file collection.hpp
 */
#ifndef COLLECTION_CLASS_H
#define COLLECTION_CLASS_H

#include "hash_index.hpp"
#include "auxiliary.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <json.hpp>
using json = nlohmann::json;



/**
 * @namespace Namespace for NoSQLite
 */
namespace nosqlite {

    /**
     * @class collection
     */
    class collection {
    private: 
        /**
         * @brief Doubles as the path and name of the collection.
         */
        std::string path;

        /**
         * @brief Number of documents in the collection.
         */
        unsigned long long number_of_documents;

        /**
         * @brief Map of indexes associated with the collection.
         */
        std::unordered_map<std::string, hash_index*> indexes;

        /**
         * @brief Turns parallel processing on and off.
         */
        bool parallel_processing;

        /**
         * @param json_content JSON document to add to the collection.
         * @param update_header Indicates whether the header file should be updated or not.
         * @brief Adds a new document to the collection, updates the indices and the header file (or not).
         */
        int add_document(const std::string &json_content, bool update_header);

        /**
         * @param json_object JSON object to add to the collection.
         * @param update_header Indicates whether the header file should be updated or not.
         * @brief Adds a new document to the collection, updates the indices and the header file (or not).
         */
        int add_document(json &json_object, bool update_header);

        int delete_single(const fs::path &file_path, const std::vector<condition_type> &conditions);

    public:

        /**
         * @param path Path and name of the collection
         * @brief Constructor for the collection class.
         */
        collection(const std::string &path);

        /**
         * @brief Destructor for the collection class.
         */
        ~collection();

        /**
         * @brief Turns on parallel processing
         */
        void turn_on_parallel_processing();

        /**
         * @brief Turns off parallel processing
         */
        void turn_off_parallel_processing();
        
        /**
         * @param path_to_json Path to the directory with the files to build the collection.
         * @brief Builds a collection from scratch according to the files in the path_to_json directory.
         * @return 0 on success and 1 otherwise.
         */
        int build_from_scratch(const std::string &path_to_json);

        /**
         * @brief Builds a collection instance from an existing collection in memory.  Uses a database (with the NoSQLite specifications) already in the file system to build the collection with the corresponding name.
         * @return 0 on success and 1 otherwise.
         */
        int build_from_existing();

        /**
         * @brief Gets the path to the collection.
         */
        std::string get_path() const;

        /**
         * @brief Gets the name of the collection.
         */
        std::string get_name() const;

        /**
         * @brief Gets the number of documents in the collection.
         */
        unsigned long long get_number_of_documents() const;

        /**
         * @param json_content JSON document to add to the collection.
         * @brief Adds a new document to the collection and updates the indices.
         */
        int add_document(const std::string &json_content);
    
        /**
         * @param new_document Name of the new JSON document to be created.
         * @brief Public wrapper to insert a new document.
         * @return Returns 0 if the JSON document was successfully created and 1 otherwise.
         */
        int create_document(const nlohmann::json &new_document);

        /**
         * @brief Gets all the documents in the collection.
         * @return Returns a vector with all the documents as JSON objects.
         */
        std::vector<json> read_all() const;

        /**
         * @param conditions List of conditions that condition the read operation.
         * @brief Gets all the documents in the collection that satisfy all of the conditions.
         */
        std::vector<json> read_with_conditions(const std::vector<condition_type> &conditions) const;

        /**
         * @param id Id of the document to
         * @param updated_data New data for the document.
         * @param final Reference to a JSON object that will be filled with the final document.
         * @brief Updated the document with the specified id.
         */
        int update_document(unsigned long long id, const json& updated_data, json &final);
        
        /**
         * @param conditions List of conditions that condition the update operation.
         * @param updated_data New data for the documents.
         * @brief Updated the documents that satisfy the given conditions.
         * @return Returns the updated documents.
         */
        std::vector<json> update_document(const std::vector<condition_type> &conditions, const json& updated_data);
        
        /**
         * @param id Id of the document to get.
         * @brief Gets a document by id.
         * @returns JSON of the document with the specified id.
         */
        json get_document(unsigned long long id) const;

        /**
         * @param field Field to be indexed if length is one and list of nested fields where the last one is indexed otherwise.
         * @brief Creates a hash index on the field parameter.
         */
        int create_hash_index(const field_type &field);

        /**
         * @param index_name Name of the index.
         * @param value Value to find in the index.
         * @brief Wrapper to consult a hash index.
         */
        std::vector<std::string> consult_hash_index(const std::string &index_name, const json &value) const;
        
        /**
         * @param conditions Vector of tuples with (field_path, operator, value)
         * @brief Deletes all documents that satisfy all conditions
         * @return Number of documents deleted or -1 if an error occurred
         */
        int delete_with_conditions(const std::vector<condition_type> &conditions);

        /**
         * @brief Deletes the entire collection from the database.
         */
        void delete_collection();

        /**
         * @param field Field of the index to find.
         * @brief Checks if and index on the collection on the specified field exists.
         */
        bool find_index(const field_type &field);

        /**
         * @param Field of the index to delete.
         * @brief Delete the hash index on the specified field.
         * @return 0 on success and 1 otherwise.
         */
        int delete_hash_index(const field_type &field);

     };
 } // namespace nosqlite
 
 #endif
