/**
 * @file collection.hpp
 */
#ifndef COLLECTION_CLASS_H
#define COLLECTION_CLASS_H

#include <string>
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
    };
    
} // namespace nosqlite

#endif