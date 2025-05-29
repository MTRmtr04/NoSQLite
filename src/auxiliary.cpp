#include <iostream>
#include <fstream>
#include <unistd.h>
#include "auxiliary.hpp"

namespace nosqlite {
    
    // TODO: Adapt for Windows file path.
    std::string get_last_dir(const std::string &path) {
        std::stringstream ss(path);
        std::string t;
        while (getline(ss, t, '/'));
        return t;
    }

    int check_path_existence(const std::string &path) {
        fs::path fs_path = path;
        if (!fs::exists(fs_path)) {
            std::cerr << "Error: Path doesn't exist. No such file or directory: \"" << path << "\"" << std::endl;
            return 1;
        }
        return 0;
    }

    std::string hash_string(const std::string &str) {
        std::hash<std::string> hash_func;
        size_t hash = hash_func(str);

        std::stringstream stream;
        stream << std::hex << hash;
        return stream.str();
    }

    std::string hash_integer(unsigned long long num) {
        return hash_string(std::to_string(num));
    }

    std::string hash_json(const json &object) {
        return hash_string(object.dump());
    }

    void throw_failed_to_open_file(fs::path path) {
        std::cerr << "Error: Failed to open file: " << path << "." << std::endl;
    }

    void throw_failed_to_create_file(fs::path path) {
        std::cerr << "Error: Failed to create file: " << path << "." << std::endl;
    }

    void throw_failed_to_create_header(const std::string &collection_name) {
        if (collection_name.empty())std::cerr << "Error: Failed to create database header file" << std::endl;
        else std::cerr << "Error: Failed to create collection header file. Collection: \"" << collection_name << "\"." << std::endl;
    }

    void throw_failed_to_update_header(const std::string &collection_name) {
        if (collection_name.empty())std::cerr << "Error: Failed to update database header file" << std::endl;
        else std::cerr << "Error: Failed to update collection header file. Collection: \"" << collection_name << "\"." << std::endl;
    }

    void throw_no_such_collection(const std::string &collection_name) {
        std::cerr << "Collection: \"" << collection_name << "\" does not exist in this database." << std::endl;
    }

    void throw_failed_to_create_collection_entries(const std::vector<fs::path> &failed_paths) {
        if(!failed_paths.empty()) {
            std::cerr << "Error: Failed to create database entries for the JSON objects at the following locations:" << std::endl;
            for (fs::path fail : failed_paths) {
                std::cerr << "    " << fail << std::endl;
            }
        }
    }

    json read_and_parse_json(fs::path path) {
        std::ifstream file(path);
        json object;
       
        if (file.is_open()) {
            try {
                object = json::parse(file);
            }
            catch(const nlohmann::json::parse_error& e) {
                std::cerr << "Error: Invalid JSON in the file at: " << path << "." << std::endl;
            }
        }
        else {
            throw_failed_to_open_file(path);
        }

        file.close();
        return object;
    }

    json read_and_parse_json(const std::string &json_content) {
        json object;
        try {
            object = json::parse(json_content);
        }
        catch(const nlohmann::json::parse_error& e) {
            std::cerr << "Error: Invalid JSON string." << std::endl;
        }
        return object;   
    }

    json access_nested_fields(json content, std::vector<std::string> fields) {
        json obj = content;
        for (const std::string &field : fields) {
            obj = obj[field];
        }
        return obj;
    }

    std::string build_index_name(const std::vector<std::string> &fields) {
        std::string name = "hash";

        for (const std::string &field : fields) {
            name +=  "_" + field;
        }
        
        return name;
    }
    
    bool compare(const json &value1, const std::string &op, const json &value2) {
        if (value1.is_array()) {
            for (const auto &val : value1) {
                if (compare(val, op, value2)) return true;
            }
            return false;
        }

        if (op == "==") return value1 == value2;
        if (op == "!=") return value1 != value2;
        if (op == ">")  return value1.is_number() && value2.is_number() && value1 > value2;
        if (op == "<")  return value1.is_number() && value2.is_number() && value1 < value2;
        if (op == ">=") return value1.is_number() && value2.is_number() && value1 >= value2;
        if (op == "<=") return value1.is_number() && value2.is_number() && value1 <= value2;
    
        return false;
    }


}