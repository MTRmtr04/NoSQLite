/**
 * @file collection.hpp
 */
#ifndef HASH_INDEX_CLASS_H
#define HASH_INDEX_CLASS_H

#include <string>
#include <json.hpp>
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

        /**
         * @param fields List of nested fields. Only the last one will be indexed.
         * @brief Builds the hash index.
         */
        void build_index(const std::vector<std::string> &fields);

    public:

        /**
         * @param path Path to the collection to be indexed.
         * @param fields List of nested fields. Only the last one will be indexed.
         * @brief Constructor for the hash_index class.
         */
        hash_index(const std::string &path, const std::vector<std::string> &fields);

        /**
         * @brief Getter for the path attribute.
         */
        std::string get_path();

        /**
         * @brief Getter for the indexed field name derived from the path attribute.
         */
        std::string get_field();
    
    };

}

#endif