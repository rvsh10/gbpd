#include <iostream>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

#include <unordered_set>
#include <unordered_map>
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

	json data;

	// first branch (fb) & second branch (sb)  packages unique names 
	std::unordered_set<std::string> fb_unique_names, sb_unique_names;

	// maps for output packages to json
	std::unordered_map<std::string, size_t> fb_packages, sb_packages;

	int fb_packages_count = 0;	
	for (const auto& package : first_branch_data["packages"]) {
		
		fb_unique_names.insert(package.value("name", ""));
		fb_packages[package.value("name", "")] = fb_packages_count++;
		
	}

	int sb_packages_count = 0;
	for (const auto& package : second_branch_data["packages"]) {

		sb_unique_names.insert(package.value("name", ""));
		sb_packages[package.value("name", "")] =  sb_packages_count++;
	}

	std::ofstream outfile_1, outfile_2, outfile_3;
	outfile_1.open("fb_names.txt", std::ios::out);
	outfile_2.open("sb_names.txt", std::ios::out);
	outfile_3.open("data.txt", std::ios::out);
  	if (!outfile_1) {
		std::cerr << "Error: unable to create or open file!" << std::endl;
	}

	if (!outfile_2) {
		std::cerr << "Error: unable to create or open file!" << std::endl;
	}

	if (!outfile_3) {
		std::cerr << "Error: unable to create or open file!" << std::endl;
	}
	
	// copy of first branch packages to find unique packages
	// in second branch
	std::unordered_set<std::string> fbun_copy = fb_unique_names, common;	
        
	// find unique packages' names 

	//find packages that's in the first branch but not in the second
	
	for (const auto& name : sb_unique_names) {
		const auto& data_to_insert = first_branch_data["packages"][fb_packages[name]];
		if (!fb_unique_names.erase(name)) {
			data["first_branch_packages"].push_back(data_to_insert); 
 
		} else {
			common.insert(name);
		}
	}	
	
	// find packages that's in the second branch but not in the first
	for (const auto& name : fbun_copy) {
		const auto& data_to_insert = second_branch_data["packages"][sb_packages[name]];
		if (!sb_unique_names.erase(name)) {
			data["second_branch_packages"].push_back(data_to_insert);
		} else {
			common.insert(name);
		}
	}

	for (const auto& name : common) {
		auto fb_packages_version = first_branch_data["packages"][fb_packages[name]].value("version", "");	
		auto sb_packages_version = second_branch_data["packages"][sb_packages[name]].value("version", "");
		if (fb_packages_version > sb_packages_version) {
			data["higher_version_release"].push_back(first_branch_data["packages"][fb_packages[name]]);
			//std::cout << fb_packages_version << " ";
		}
		
	

	}

	std::cout << "\n";
	

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

	outfile_3 << data.dump(4);

	outfile_1.close();
	outfile_2.close();	
        

	// freeing up resources
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();

    return 0;
}


