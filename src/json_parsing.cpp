#include "json_parsing.hpp"


nlohmann::json simdjson_to_nlohmann(simdjson::ondemand::document &simdjson) {   
    return nlohmann::json::parse(simdjson::to_json_string(simdjson).value());
}