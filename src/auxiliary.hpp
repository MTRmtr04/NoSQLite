/**
 * @file auxiliary.hpp
 */
#ifndef AUXILIARY_H
#define AUXILIARY_H

#include <filesystem>
#include <json.hpp>

namespace fs = std::filesystem;
using json = nlohmann::json;


namespace nosqlite {
    /**
     * @param path File path with to a directory without trailing /.
     * @brief Extracts the last directory in a file path.
     * @return Name of the last directory in path.
     */
    std::string get_last_dir(const std::string &path);

    /**
     * @param path File path to verify.
     * @brief Checks if the path exists in the file system.
     * @return Return pointer to the path object if the path exists and nullptr otherwise.
     */
    int check_path_existence(const std::string &path);

    /**
     * @param str String to be hashed.
     * @brief Hashes a string and returns a hexadecimal string.
     */
    std::string hash_string(const std::string &str);

    /**
     * @param num Integer to be hashed.
     * @brief Hashes an Integer and returns a hexadecimal string.
     */
    std::string hash_integer(unsigned long long num);

    /**
     * @param object JSON object to be hashed.
     * @brief Hashes a JSON object and returns a hexadecimal string.
     */
    std::string hash_json(const json &object);

    /**
     * @param path Path to file.
     * @brief Prints out an error message saying the file didn't open properly.
     */
    void throw_failed_to_open_file(fs::path path);

    /**
     * @param path Path to file.
     * @brief Prints out an error message saying the file couldn't be created.
     */
    void throw_failed_to_create_file(fs::path path);

    /**
     * @param collection_name Name of the collection whose header couldn't be created. Default value is empty string for the database header.
     * @brief Prints out an error message saying the header file couldn't be created.
     */
    void throw_failed_to_create_header(const std::string &collection_name = "");

    /**
     * @param collection_name Name of the collection whose header couldn't be updated. Default value is empty string for the database header.
     * @brief Prints out an error message saying the header file couldn't be updated.
     */
    void throw_failed_to_update_header(const std::string &collection_name = "");

    /**
     * @param collection_name Name of the collection.
     * @brief Prints error message saying the collection doesn't exist in the database.
     */
    void throw_no_such_collection(const std::string &collection_name);

    /**
     * @param failed_paths List of failed paths.
     * @brief Prints an error messages saying that for the paths in the list no database entry could be created.
     */
    void throw_failed_to_create_collection_entries(const std::vector<fs::path> &failed_paths);

    /**
     * @param path Path to JSON file to be read an parsed.
     * @brief Reads the JSON file into a json object or throws an error if the JSON is invalid.
     * @return A json object or nullptr.
     */
    json read_and_parse_json(fs::path path);

    /**
     * @param json_content JSON string to be read an parsed.
     * @brief Reads the JSON string into a json object or throws an error if the JSON is invalid.
     * @return A json object or nullptr.
     */
    json read_and_parse_json(const std::string &json_content);

    /**
     * @param content JSON object.
     * @param fields Nested fields to access.
     * @brief Access the json content inside the nested field that is the last element of the fields vector.
     * @return A JSON object with the content of the field should it exist and null otherwise. Should no fields be provided the original JSON is returned.
     */
    json access_nested_fields(json content, std::vector<std::string> fields);

    /**
     * @param fields List of fields
     * @brief Builds the name of the hash index
     */
    std::string build_index_name(const std::vector<std::string> &fields);

    /**
     * @param value1 The first value.
     * @param op The operation
     * @param value2 The second value.
     * @brief Compares the too values according to the operation.
     * @return The boolean result of the comparison.
     */
    bool compare(const json &value1, const std::string &op, const json &value2);

    /**
     * @param all_results Vector of vectors with the results.
     * @param results Destination of the pooled results.
     * @brief Pools all of the values in each of the vector of all_results and returns them in a single vector by reference.
     */
    void pool_results(const std::vector<std::vector<json>> &all_results, std::vector<json> &results);

    /**
     * @param collection_path Path to the directory.
     * @param paths Vector where the paths will be collected.
     * @brief Collects all of the paths to database files in the directory collection_path.
     */
    void collect_paths(const std::string &collection_path, std::vector<fs::path> &paths);

}

#endif
