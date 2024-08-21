#include "json_fetch.h"
#include <iostream>
#include <curl/curl.h>
#include <unordered_set>
#include <unordered_map>

using json = nlohmann::json;


// call back function for data write
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// fetch json with pakcages from branch
json fetch_json(std::string branch) {
    
	CURL* curl;
    	CURLcode res;
    	std::string readBuffer;
	json branch_data;

	curl = curl_easy_init();
    	if(curl) {

		std::string api = "https://rdb.altlinux.org/api/export/branch_binary_packages/";

       		// set callback function for data write
        	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);

        	// set buffer
        	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        	// set get request
        	curl_easy_setopt(curl, CURLOPT_URL, (api + branch).c_str());
        
		// exe reuquest
        	res = curl_easy_perform(curl);

        	if (res != CURLE_OK) {
            		std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        	} 
	
		try {
			branch_data = json::parse(readBuffer);

		} catch (json::parse_error& e) {
			std::cerr << "JSON parse error: " << e.what() << std::endl;
		}


	}

	curl_easy_cleanup(curl);
	return branch_data;
}


json get_data(const std::string& first_branch, const std::string& second_branch) {
    	
	// json with all data
	json data = {};

	json first_branch_data = fetch_json(first_branch);
	json second_branch_data = fetch_json(second_branch);
	
	std::unordered_map<std::string, std::unordered_map<std::string, size_t>> first_branch_packages;
	std::unordered_map<std::string, std::unordered_map<std::string, size_t>> second_branch_packages;
	std::unordered_map<std::string, std::unordered_set<std::string>> common_packages;
	
	int fb_data_counter = 0;
	for (const auto& package : first_branch_data["packages"]) {
		
		const auto& arch = package.value("arch", "");
		const auto& package_name = package.value("name", "");

		first_branch_packages[arch][package_name] = fb_data_counter++;
	}

	int sb_data_counter = 0;
	for (const auto& package : second_branch_data["packages"]) {
		
		const auto& arch = package.value("arch", "");
		const auto& package_name = package.value("name", "");

		second_branch_packages[arch][package_name] = sb_data_counter++;
	}
	

	// find unique packages in first branch
	// and insert higher release version
	for (const auto& archs_pnames_idxs : second_branch_packages) {
		const auto& arch = archs_pnames_idxs.first;
		const auto& pnames_idxs = archs_pnames_idxs.second; // pair
		
		json arch_packs = {};
		for (const auto& names_idxs : pnames_idxs) {
			const auto& name = names_idxs.first;
			
			if (first_branch_packages[arch].find(name) != 
					first_branch_packages[arch].end()) {
				const auto& fp_idx = first_branch_packages[arch][name];	
				const auto& sp_idx = second_branch_packages[arch][name];	
				const auto& fb_package_version = first_branch_data["packages"][fp_idx].value("version", "");
				const auto& sb_package_version = second_branch_data["packages"][sp_idx].value("version", "");
				if (fb_package_version > sb_package_version) {
					arch_packs[arch].push_back(first_branch_data["packages"][fp_idx]);
				}
				
				common_packages[arch].insert(name);
				first_branch_packages[arch].erase(name);
			}

		}
		if (!arch_packs.empty()) {
			data["higher_version_release"].push_back(arch_packs);
		}
	}


	// second branch unique packages
	for (const auto& archs_pnames_idxs : common_packages) {
		const auto& arch = archs_pnames_idxs.first;
		const auto& pnames = archs_pnames_idxs.second; // pair
		
		for (const auto& name: pnames) {
			second_branch_packages[arch].erase(name);
		}
	}
	
	
	// insert unique second branch packages	
	for (const auto& archs_pnames_idxs : second_branch_packages) {
		const auto& arch = archs_pnames_idxs.first;
		const auto& pnames_idxs = archs_pnames_idxs.second; // pair

		json arch_packs = {};
		for (const auto& names_idxs : pnames_idxs) {
			const auto& idx = names_idxs.second;
			arch_packs[arch].push_back(second_branch_data["packages"][idx]);
		}

		if (!arch_packs.empty()) {
			data["second_branch_unique"].push_back(arch_packs);
		}

	}

	

	// insert unique first branch packages	
	for (const auto& archs_pnames_idxs : first_branch_packages) {
		const auto& arch = archs_pnames_idxs.first;
		const auto& pnames_idxs = archs_pnames_idxs.second; // pair

		json arch_packs = {};
		for (const auto& names_idxs : pnames_idxs) {
			const auto& idx = names_idxs.second;
			arch_packs[arch].push_back(first_branch_data["packages"][idx]);
		}

		if (!arch_packs.empty()) {
			data["first_branch_unique"].push_back(arch_packs);
		}
	}

	return data;
}


