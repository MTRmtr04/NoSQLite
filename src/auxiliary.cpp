#include <iostream>
#include "auxiliary.hpp"


// TODO: Adapt for Windows file path.
std::string nosqlite::get_last_dir(std::string path) {
    std::stringstream ss(path);
    std::string t;
    while (getline(ss, t, '/'));
    return t;
}

int nosqlite::check_path_existence(const std::string &path) {
    fs::path fs_path = path;
    if (!fs::exists(fs_path)) {
        std::cerr << "Error: Path doesn't exist. No such file or directory: \"" << path << "\"" << std::endl;
        return 1;
    }
    return 0;
}

std::string nosqlite::hash_integer(unsigned long long num) {
    std::hash<unsigned long long> hash_func;
    size_t hash = hash_func(num);

    std::stringstream stream;
    stream << std::hex << hash;
    return stream.str();
}
