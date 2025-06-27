#include <iostream>
#include <fstream>
#include "auxiliary.hpp"

namespace nosqlite {
    
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
        if (collection_name.empty()) std::cerr << "Error: Failed to create database header file" << std::endl;
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

    json access_nested_fields(json content, field_type fields) {
        json obj = content;
        for (const std::string &field : fields) {
            obj = obj[field];
        }
        return obj;
    }

    bool find_nested_field(json content, field_type field) {
        if (field.size() == 1) return content.find(field[0]) != content.end();
        else if (field.size() == 0) return false;
        else {
            auto obj = content.find(field[0]);
            if (obj == content.end()) return false;
            else {
                field.erase(field.begin());
                return find_nested_field((*obj), field);
            }
        }
    }

    std::vector<std::string> build_possible_index_names(const json &data, const std::string &prefix) {
        std::vector<std::string> index_names;

        for (auto it = data.begin(); it != data.end(); ++it) {
            std::string field_name = prefix + it.key();
            index_names.push_back(field_name);
            if (it.value().is_object()) {
                std::vector<std::string> nested_names = build_possible_index_names(it.value(), field_name + "_");
                index_names.insert(index_names.end(), nested_names.begin(), nested_names.end());
            }
        }

        return index_names;
    }

    std::string build_index_name(const field_type &fields) {
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

    void pool_results(const std::vector<std::vector<json>> &all_results, std::vector<json> &results) {

        for (const std::vector<json> &res : all_results) {
            results.insert(results.end(), std::make_move_iterator(res.begin()), std::make_move_iterator(res.end()));
        }
    }

    void collect_paths(const std::string &collection_path, std::vector<fs::path> &paths) {
        for (const fs::path &file_path : fs::recursive_directory_iterator(collection_path)) {
            if (file_path.extension() != ".json" || file_path.filename() == "header.json" || file_path.filename() == "index.json") continue; 
            paths.push_back(file_path);
        }
    }


}
