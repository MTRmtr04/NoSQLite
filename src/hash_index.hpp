/**
 * @file collection.hpp
 */
#ifndef HASH_INDEX_CLASS_H
#define HASH_INDEX_CLASS_H

#include <string>
#include <json.hpp>
#include "auxiliary.hpp"
using json = nlohmann::json;


/**
 * @namespace Namespace for NoSQLite
 */
namespace nosqlite {

    class hash_index {
    private:

        /**
         * @brief Doubles as the path to the index files and the name of the indexed field.
         */
        std::string path;

    public:

        /**
         * @param path Path to the index.
         * @brief Constructor for the hash_index class. Constructs the index object of an index already in memory.
         */
        hash_index(const std::string path);

        /**
         * @param fields List of nested fields. Only the last one will be indexed.
         * @brief Builds the hash index.
         */
        int build_index(const field_type &fields);

        /**
         * @brief Getter for the path attribute.
         */
        std::string get_path();

        /**
         * @brief Getter for the indexed field name derived from the path attribute.
         */
        std::string get_field();

        /**
         * @param value Value of the indexed field.
         * @return List of the paths to the files containing the documents with the fields that hash to the parameter value.
         * @brief Consults the index to get the paths to the documents with the coresponding hash.
         */
        std::vector<std::string> consult(const json &value);

        /**
         * @brief Deletes this index.
         */
        void delete_index();
    
        /**
         * @param original_value Original indexed value.
         * @param updated_value Updated indexed value.
         * @param document_path Path to the updated document.
         * @brief Updates the hash index. Should be called after updating a value for an indexed field.
         */
        void update_index(json original_value, json updated_value, const std::string &document_path);
        
        /**
         * @param new_value New indexed value.
         * @param document_path Path to the updated document.
         * @brief Updates the hash index. Should be called after updating a value for an indexed field.
         */
        void update_index(json new_value, const std::string &document_path);

    };

}

#endif