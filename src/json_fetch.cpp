#include <iostream>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;


// call back function for data write
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

int main() {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if(curl) {
        // set get request
        curl_easy_setopt(curl, CURLOPT_URL, "https://rdb.altlinux.org/api/export/branch_binary_packages/p10?arch=i586");

        // set callback function for data write
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);

        // set buffer
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // exe reuquest
        res = curl_easy_perform(curl);

        if(res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        } else {
            // try to parse JSON
            try {
                auto jsonData = json::parse(readBuffer);

		std::cout << (jsonData["packages"][2].value("version", "") > 
			jsonData["packages"][1].value("version", "")) << std::endl;	



            } catch (json::parse_error& e) {
                std::cerr << "JSON parse error: " << e.what() << std::endl;
            }
        }

	


        // Освобождаем ресурсы
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();

    return 0;
}

