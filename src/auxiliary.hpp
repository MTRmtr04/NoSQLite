/**
 * @file auxiliary.hpp
 */
#ifndef AUXILIARY_H
#define AUXILIARY_H

#include <filesystem>
#include "json.hpp"

namespace fs = std::filesystem;
using json = nlohmann::json;


namespace nosqlite {
    /**
     * @param path File path with to a directory without trailing /.
     * @brief Extracts the last directory in a file path.
     * @return Name of the last directory in path.
     */
    std::string get_last_dir(std::string path);

    /**
     * @param path File path to verify.
     * @brief Checks if the path exists in the file system.
     * @return Return pointer to the path object if the path exists and nullptr otherwise.
     */
    int check_path_existence(const std::string &path);

    /**
     * @param num Integer to be hashed.
     * @brief Hashes an Integer and return a hexadecimal string,
     */
    std::string hash_integer(unsigned long long num);

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
    void throw_failed_to_create_header(std::string collection_name = "");

    /**
     * @param collection_name Name of the collection whose header couldn't be updated. Default value is empty string for the database header.
     * @brief Prints out an error message saying the header file couldn't be updated.
     */
    void throw_failed_to_update_header(std::string collection_name = "");

    /**
     * @param collection_name Name of the collection.
     * @brief Prints error message saying the collection doesn't exist in the database.
     */
    void throw_no_such_collection(std::string collection_name);

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
    json read_and_parse_json(std::string json_content);
}

#endif
