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
         * @brief Map of collections.
         */
        std::unordered_map<std::string, collection*> collections;

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
         * @brief Destructor for the database class.
         */
        ~database();

        /**
         * @brief Turns on parallel processing
         */
        void turn_on_parallel_processing();

        /**
         * @brief Turns off parallel processing
         */
        void turn_off_parallel_processing();

        /**
        * @param path_to_json Path to the json files that will build the database. Must be a directory. Each subdirectory will be taken as an individual collection.
        * @brief Build a database from scratch using the json files in path_to_json. Will delete everyting in the path directory.
        * @return 0 on success and 1 otherwise.
        */
        int build_from_scratch(const std::string &path_to_json);

        /**
         * @brief Builds the database instance from a database already in memory (must follow the NoSQLite specifications)
         * @return 0 on success and 1 otherwise.
         */
        int build_from_existing();

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
        int create_hash_index(const std::string &col_name, const field_type &field);

        /**
         * @param col_name Name of the collection.
         * @param document JSON of the document.
         * @brief Creates a document in the specified collection.
         * @return Returns 0 if the JSON document was successfully created and 1 otherwise.
         */
        int create_document(const std::string &col_name, json &document);

        /**
         * @param col_name Name of the collection.
         * @param conditions These values impose conditions on the documents that are read.
         * @brief Reads from the database and returns the documents according to the conditions.
         * @return Returns a vector with the results.
         */
        std::vector<json> read(const std::string &col_name, const std::vector<condition_type> &conditions = {});

        /**
         * @param col_name Name of the collection.
         * @param conditions These values impose conditions on the documents that are updated.
         * @param updated_data New data for the documents.
         * @brief Updates the documents in the database according to the conditions and update data.
         * @return Returns the updated documents.
         */
        std::vector<json> update(const std::string &col_name, const std::vector<condition_type> &conditions, const json &updated_data);

        /**
         * @param col_name Name of the collection.
         * @param conditions These values impose conditions on the documents that are deleted.
         * @brief Deletes documents in the database according to the conditions.
         * @return Returns the number of deleted documents.
         */
        int remove(const std::string &col_name, const std::vector<condition_type> &conditions = {});

        /**
         * @param col_name Name of the collection.
         * @brief Deletes a collection from the database.
         * @return 0 on success and 1 otherwise 
         */
        int delete_collection(const std::string &col_name);

        /**
         * @param col_name Name of the collection.
         * @param path_to_files Path to JSON files for the new collection.
         * @brief Creates from the files in the path_to_files or an empty collection if the path is an empty string.
         * @return 0 on success and 1 otherwise.
         */
        int create_collection(const std::string &col_name, const std::string &path_to_files = "");

        /**
         * @param col_name Name of the collection.
         * @param field Indexed field for the index to be deleted.
         * @brief Delete the the index from the specified collection on the specified field.
         * @return 0 on success and 1 otherwise
         */
        int delete_hash_index(const std::string &col_name, const field_type &field);
    
       
    };
} // namespace nosqlite

#endif
