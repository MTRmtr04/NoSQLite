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
         * @brief Builds the hash index.
         */
        void build_index();

    public:

        /**
         * @brief Constructor for the hash_index class.
         */
        hash_index(const std::string &path);

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