#include <iostream>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

#include <unordered_set>
#include <fstream>


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
        // set callback function for data write
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);

        // set buffer
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // set get request
        curl_easy_setopt(curl, CURLOPT_URL, "https://rdb.altlinux.org/api/export/branch_binary_packages/p10");
        
	// exe reuquest
        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            	std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        } 
	
	json first_branch_data;
	try {
		first_branch_data = json::parse(readBuffer);

	} catch (json::parse_error& e) {
		std::cerr << "JSON parse error: " << e.what() << std::endl;
	}

	readBuffer.clear();

	curl_easy_setopt(curl, CURLOPT_URL, "https://rdb.altlinux.org/api/export/branch_binary_packages/p9");
	res = curl_easy_perform(curl);

	if (res != CURLE_OK) {
		std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
	}

	json second_branch_data;
	try {
		second_branch_data = json::parse(readBuffer);

	} catch (json::parse_error& e) {
                std::cerr << "JSON parse error: " << e.what() << std::endl;
            
        }
	std::cout << "All JSON data parsed\n";

	// first branch (fb) & second branch (sb)  packages unique names 
	std::unordered_set<std::string> fb_unique_names, sb_unique_names;
	
	for (const auto& package : first_branch_data["packages"]) {
		
		fb_unique_names.insert(package.value("name", ""));

	}
	for (const auto& package : second_branch_data["packages"]) {

		sb_unique_names.insert(package.value("name", ""));

	}
	std::ofstream outfile_1, outfile_2;
	outfile_1.open("fb_names.txt", std::ios::out);
	outfile_2.open("sb_names.txt", std::ios::out);
  	if (!outfile_1) {
		std::cerr << "Error: unable to create or open file!" << std::endl;
	}

	if (!outfile_2) {
		std::cerr << "Error: unable to create or open file!" << std::endl;
	}

	std::unordered_set<std::string> fbun_copy = fb_unique_names;	

	// find unique packages' names 

	//find packages that's in the first branch but not in the second
	for (const auto& name : sb_unique_names) {
		fb_unique_names.erase(name);
	}	
	
	// find packages that's in the second branch but not in the first
	for (const auto& name : fbun_copy) {
		sb_unique_names.erase(name);
	}
	

	// output unique names from first branch
	for (const auto& name : fb_unique_names) {
		outfile_1 << name << " ";
	}	
	outfile_1 << "\n";
	
	// output unique names from second branch
	for (const auto& name : sb_unique_names) {
		outfile_2 << name << " ";
	}
	outfile_2 << "\n";

	outfile_1.close();
	outfile_2.close();	
        

	// freeing up resources
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();

    return 0;
}

