#include "auxiliary.hpp"


nlohmann::json simdjson_to_nlohmann(simdjson::ondemand::document &simdjson) {   
    return nlohmann::json::parse(simdjson::to_json_string(simdjson).value());
}

// TODO: Adapt for Windows file path.
std::string get_last_dir(std::string path) {
    std::stringstream ss(path);
    std::string t;
    while (getline(ss, t, '/'));
    return t;
}
