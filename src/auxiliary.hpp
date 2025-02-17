/**
 * @file auxiliary.hpp
 */
#ifndef AUXILIARY_H
#define AUXILIARY_H

#include "json.hpp"
#include "simdjson.h"

/**
 * @param simdjson simdjson ondemand document instance.
 * @brief Converts a simdjson ondemand document instance into a nlohman json instance.
 * @return nlohmann json instance for editing the json.
 */
nlohmann::json simdjson_to_nlohmann(simdjson::ondemand::document &simdjson);

/**
 * @param path File path with to a directory without trailing /.
 * @brief Extracts the last directory in a file path.
 * @return Name of the last directory in path.
 */
std::string get_last_dir(std::string path);

#endif
