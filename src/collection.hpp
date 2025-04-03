/**
 * @file collection.hpp
 */
#ifndef COLLECTION_CLASS_H
#define COLLECTION_CLASS_H

#include "hash_index.hpp"
#include <string>
#include <vector>
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
       
         /**
          * @param new_document Name of the new JSON document to be created.
          * @brief Public wrapper to insert a new document.
          * @return Returns 0 if the JSON document was successfully created and 1 otherwise.
          */
         int create_document(const nlohmann::json &new_document);
       
         /**
          * @param id ID of the document to be retrieved.
          * @brief Recovers a document from the collection using its ID.
          * @return The document JSON founded or an empty JSON object if it doesn't exist.
          */
         json get_document(unsigned long long id) const;
 
         /**
          * brief Finds all documents in the collection.
          * @return A vector of JSON objects.
          */
         std::vector<json> get_all_documents() const;
 
         /**
          * @brief Finds all documents in the collection.
          * @return A vector of JSON objects.
          */
         std::vector<json> find_all() const;
 
         /**
          *
          * @param title Movie title to search for.
         * @brief Finds all documents in the collection with the given title.
          * @return A vector of movies matching the title.
          */
         std::vector<json> find_by_title(const std::string &title) const;
 
         /**
          * @param genre Movie genre to search for.
          * @brief Finds all documents in the collection with the given genre.
          * @return A vector of movies matching the genre.
          */
         std::vector<json> find_by_genre(const std::string &genre) const;
 
         /**
          * @param director Movie director to search for.
          * @brief Finds all documents in the collection with the given director.
          * @return Vector of movies by the specified director.
          */
         std::vector<json> find_by_director(const std::string &director) const;
 
         /**
          * @param year Year to search for.
          * @brief Finds mpvies released in the specified year.
          * @return Vector of movies released in the specified year.
          */
         std::vector<json> find_by_year(int year) const;
 
        /**
         * @param id ID of document to be retrieved.
         * @brief Public wrapper to read a document by its id
         * @return The document JSON founded or an empty JSON object if it doesn't exist.
         */
        std::string path;

        /**
         * @brief Number of documents in the collection.
         */
        unsigned long long number_of_documents;

        /**
         * @brief List if indexes associated with the collection.
         */
        std::vector<hash_index> indexes;

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
         * @param field Field to be indexed
         * @brief Creates a hash index on the field parameter or on a nested field should there be any more parameters
         */
        void create_hash_index(const std::string &field, ...);

        json read_document_by_id(unsigned long long id);
 
     };
 } // namespace nosqlite
 
 #endif
