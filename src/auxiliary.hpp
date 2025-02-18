/**
 * @file auxiliary.hpp
 */
#ifndef AUXILIARY_H
#define AUXILIARY_H

#include <filesystem>
#include "json.hpp"

namespace fs = std::filesystem;

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
}

#endif
