/**
 * @file database.hpp
 */
#ifndef DATABASE_CLASS_H
#define DATABASE_CLASS_H

#include <string>
#include <unordered_map>
#include "collection.hpp"
#include "auxiliary.hpp"


/**
 * @namespace Namespace for NoSQLite
 */
namespace nosqlite {

    /**
     * @class database
     */
    class database {

    private:
        /**
         * @brief Path to the json file database.
         */
        std::string path;

        /**
         * @brief 
         */
        std::unordered_map<std::string, collection*> collections;

        /**
        * @param path_to_json Path to the json files that will build the database. Must be a directory. Each subdirectory will be taken as an individual collection.
        * @brief Build a database from scratch using the json files in path_to_json. Will delete everyting in the path directory.
        */
        void build_from_scratch(const std::string &path_to_json);

        /**
         * @brief Builds the database instance from a database already in memory (must follow the NoSQLite specifications)
         */
        void build_from_existing();

        /**
         * @param collection_name Name of the collection.
         * @param col Pointer to the collection.
         * @brief Sets the collection instance with the correct name.
         */
        void set_collection(const std::string &collection_name, collection* col);
    public:

        /**
         * @param path Path to the json file database.
         * @brief Constructor for the database class. Uses a database (with the NoSQLite specifications) already in the file system.
         */
        database(const std::string &path);

        /**
         * @param path_to_database Path to the json file database to be created. Must be an empty directory.
         * @param path_to_json Path to the json files that will build the database. Must be a directory. Each subdirectory will be taken as an individual collection.
         * @brief Constructor for the database class. Builds a database from scratch using the json files in path_to_json. 
         */
        database(const std::string &path_to_database, const std::string &path_to_json);

        /**
         * @brief Destructor for the database class.
         */
        ~database();

        /**
         * @brief Getter for the path attribute.
         */
        std::string get_path() const;

        /**
         * @param collection Name of the collection to get.
         * @brief Get a collection instance by name.
         * @return Pointer to the collection or nullptr if it doesn't exist.
         */
        collection* get_collection(const std::string &col) const;
        
        /**
         * @param col_name Name of the collection.
         * @param field Field to be indexed if length is one and list of nested fields where the last one is indexed otherwise.
         * @brief Create an index on the collection and field in the parameters.
         */
        void create_hash_index(const std::string &col_name, const field_type &field);
       
    };
} // namespace nosqlite

#endif