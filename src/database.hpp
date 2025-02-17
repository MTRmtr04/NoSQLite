/**
 * @file database.hpp
 */
#ifndef DATABASE_CLASS_H
#define DATABASE_CLASS_H

#include <string>


/**
 * @namespace Namespace for NoSQLite
 */
namespace nosqilte {

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
        * @param path_to_json Path to the json files that will build the database. Must be a directory. Each subdirectory will be taken as an individual collection.
        * @brief Build a database from scratch using the json files in path_to_json.
        */
        void build_from_scratch(const std::string &path_to_json);
    public:

        /**
         * @param path Path to the json file database.
         * @brief Constructor for the database class. Uses a database (with the NoSQLite specifications) already in the file system.
         */
        database(const std::string &path);

        /**
         * @param path_to_database Path to the json file database to be created. Must be an empty directory.
         * @param path_to_json Path to the json files that will build the database. Must be a directory. Each subdirectory will be taken as an individual collection.
         * @brief Constructor for the database class. Build a database from scratch using the json files in path_to_json. 
         */
        database(const std::string &path_to_database, const std::string &path_to_json);

        /**
         * @brief Destructor for the database class.
         */
        ~database();

        /**
         * @brief Getter for the path attribute.
         */
        std::string get_path();

       
    };
} // namespace nosqlite

#endif