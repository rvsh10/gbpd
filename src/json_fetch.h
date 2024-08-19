#ifndef JSON_FETCH_LIB
#define JSON_FETCH_LIB

#include <string>
#include <nlohmann/json.hpp>


size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);

nlohmann::json get_data(const std::string& first_branch, const std::string& second_branch);

#endif
