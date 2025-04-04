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
         * @param path_to_json Path to the directory with the files to build the collection.
         * @brief Builds a collection from scratch according to the files in the path_to_json directory.
         */
        void build_from_scratch(const std::string &path_to_json);

        /**
         * @brief Builds a collection instance from an existing collection in memory.
         */
        void build_from_existing();

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

    public:

        /**
         * @param path Path and name of the collection
         * @brief Constructor for the collection class. Uses a database (with the NoSQLite specifications) already in the file system to build the collection with the corresponding name.
         */
        collection(const std::string &path);

        /**
         * @param path Path and name of the collection
         * @param path_to_json Path to the directory with the files to build the collection.
         * @brief Constructor for the collection class. Builds a collection from scratch according to the files in the path_to_json directory.
         */
        collection(const std::string &path, const std::string &path_to_json);

        /**
         * @brief Destructor for the collection class.
         */
        ~collection();

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
         * @param field Name of the fields to search for.
         * @param value Value of the field to macth.
         * @brief Searches for a document in the collection with the given field and value.
         * @return Returns a vector of JSON objects that match the search.
         */
        std::vector<json> read(const std::string &field, const json &value) const;

        /**
         * @param field First field. Field to be indexed if only one argument is given.
         * @param fields All fields, in nesting order, required to reach the desired (last) field.
         * @brief Creates a hash index on the field parameter or on a nested field should there be any more parameters.
         */
        template<typename... Fields>
        void create_hash_index(const std::string &field, Fields... fields) {
            // TODO: Error handling. Incompatible types
            std::vector<std::string> all_fields = {field, fields...};

            std::string name = build_index_name(all_fields);
            std::string path = this->path + "/indexes/" + name;
            
            hash_index* index = new hash_index(path, all_fields);

            this->indexes[name] = index;
        }
 
     };
 } // namespace nosqlite
 
 #endif
