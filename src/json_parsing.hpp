#ifndef JSON_PARSING_H
#define JSON_PARSING_H

#include "json.hpp"
#include "simdjson.h"


nlohmann::json simdjson_to_nlohmann(simdjson::ondemand::document &simdjson);

#endif
