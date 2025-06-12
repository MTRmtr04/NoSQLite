/**
 * @file nosqlite.hpp
 */
#ifndef NOSQLITE_CLASS_H
#define NOSQLITE_CLASS_H

#include "auxiliary.hpp"
#include "database.hpp"
#include "collection.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <json.hpp>
using json = nlohmann::json;


/**
 * @namespace Namespace for NoSQLite
 */
namespace nosqlite {

    enum query_type {NONE, CREATE, READ, UPDATE, DELETE, CREATE_INDEX, DELETE_INDEX, CREATE_COLLECTION, DELETE_COLLECTION};

    /**
     * @class nosqlite
     * @brief API visible to the user.
     */
    class nosqlite_api {
    private:
        database* db;
        std::vector<condition_type> conditions;
        std::string active_collection;
        field_type active_field;
        query_type active_query_type;
        json active_json;
        std::string active_path;

        void clear_all();

        bool valid_condition(condition_type condition);

    public:

        /**
         * @param path Path to the json file database.
         * @brief Constructor for the database class. Sets up the API using a database (with the NoSQLite specifications) already in the file system.
         */
        nosqlite_api(const std::string &path);

        /**
         * @param path_to_database Path to the json file database to be created. Must be an empty directory.
         * @param path_to_json Path to the json files that will build the database. Must be a directory. Each subdirectory will be taken as an individual collection.
         * @brief Constructor for the nsqlite class. Sets up the API by creating a database from scratch
         */
        nosqlite_api(const std::string &path_to_database, const std::string &path_to_json);

        /**
         * @brief Destructor for the API.
         */
        ~nosqlite_api();

        /**
         * @param results Reference for the results of the query.
         * @brief Executes the query that was build up to the point of execution.
         * @return Return 0 if the query was succesfull or 1 otherwise.
         */
        int execute(std::vector<json> &results);

        /**
         * @param col_name Name of the collection.
         * @param document JSON of the document to be created.
         * @brief Sets up the creation of a document in the specified collection with the specified JSON.
         * @return Returns self.
         */
        nosqlite_api* create(std::string col_name, json document);

        /**
         * @param col_name Name of the collection.
         * @param condition The operation will be conditioned according to this value.
         * @brief Sets up the read operation of the specified collection with the specified condition.
         * @return Returns self.
         */
        nosqlite_api* read(std::string col_name, condition_type condition = empty_condition);

        /**
         * @param col_name Name of the collection.
         * @param update_data JSON with the data to update the documents with.
         * @param condition The operation will be conditioned according to this value.
         * @brief Sets up the read operation of the specified collection with the specified condition.
         * @return Returns self.
         */
        nosqlite_api* update(std::string col_name, json update_data, condition_type condition = empty_condition);

        /**
         * @param condition The operation will be conditioned according to this value.
         * @brief Adds a condition to the operation that is being build.
         * @return Returns self.
         */
        nosqlite_api* AND(condition_type condition);

        /**
         * @param col_name Name of the collection.
         * @brief Sets up the deletion of a collection from the database.
         * @return Returns self.
         */
        nosqlite_api* delete_collection(const std::string &col_name);

        /**
         * @param col_name Name of the collection.
         * @param path_to_json Path to the JSON files for the new collection.
         * @brief Sets up the creation of a new collection in the database from the JSON files at the specified path or an empty collection if the path is a empty string.
         * @return Returns self.
         */
        nosqlite_api* create_collection(const std::string &col_name, const std::string &path_to_json = "");

        /**
         * @param col_name Name of the collection.
         * @param field Field of the index to be deleted.
         * @brief Sets up the deletion of an index on the specified field and collection.
         * @return Returns self.
         */
        nosqlite_api* delete_index(const std::string &col_name, const field_type &field);
        
        /**
         * @param col_name Name of the collection.
         * @param field Field of the index to be created.
         * @brief Sets up the creation of an index on the specified field and collection.
         * @return Returns self.
         */
        nosqlite_api* create_index(const std::string &col_name, const field_type &field);

    };

 } // namespace nosqlite
 
 #endif
