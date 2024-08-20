#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include "json_fetch.h"
#include <unordered_set>

int main (int argc, char* argv[]) {
	
	if (argc != 3) {
		std::cerr << "Error: wrong number of arguments!\n";
		return 1;
	}

	std::string first_branch = argv[1];
	std::string second_branch = argv[2];

	std::unordered_set<std::string> branches = {
		"sisyphus",
       		"p11",
        	"p10",
        	"p9"
	};	

	if (branches.find(first_branch) == branches.end() 
		|| branches.find(second_branch) == branches.end()) {
		std::cerr << "Error: invalid arguments!\n";
		return 1;
	}

	std::cout << (get_data(first_branch, second_branch)).dump(4);

			
}
