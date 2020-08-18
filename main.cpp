#include <regex>
#include <iostream>
#include <string>
#include <string_view>
#include <filesystem>
#include <fstream>
#include <cassert>

// N0la rcon lib - https://github.com/n0la/rcon
// #include "rcon/rcon.h"

// Discord Library - https://github.com/zeroxs/aegis.cpp
// #include <aegis.hpp>

// TOML Parsing Lib
#include "tomlplusplus/toml.hpp"

using std::regex;
using std::string;
using std::string_view;
using std::cout;
using std::smatch;
using std::filesystem::exists;
using namespace std::literals;

const int TIMEOUT {5};
const std::filesystem::path confpath {"mcdown.yaml"};
const string cp = "mcdown.toml";

int main(int argc, char *argv[]){
	// Read RCON configuration file
	auto config = toml::parse_file(cp);
	string_view serveraddr = config["rcon"]["addr"].value_or(""sv);
	cout << "Server addr: " << serveraddr << '\n';
	/*
	string line;
	assert(exists(confpath));
	std::ifstream cf {confpath};
	while (std::getline(cf, line)){
		cout << line << '\n';
	};
	*/

	// Open RCON connection

	// Regex number of players
	// There are 0 of a max of 20 players online:
	string response { R"(There are 12 of a max of 20 players online:)" };
	int playercount = 0;
	regex pat{ R"((?:There are )([0-9]+))" };
	smatch players;
	regex_search(response, players, pat);
	playercount = stoi(players[1]);
	cout << "There are: " << players[1] << " players online\n";
	cout << "The whole match is: " << players[0] << "\n";
	printf("Formatted as int: %i \t Formatted as hex: %#0.2x \n", playercount, playercount);
	if ( exists("server.empty") ) {
		cout << "Server has been empty for " << TIMEOUT << " minutes. Initiating shutdown.\n";
	} else {
		cout << "Server is active.\n";
	}

/*
	// Touch or remove server empty flag file
	if (playercount == 0 && exists("server.empty")){
		system("shutdown -h now");
		cout << "Server is empty. Will shutdown in 5 minutes"
		// notify discord
	} else if (playercount == 0){
		// touch server.empty;
		// notify discord
	} else {
		remove("server.empty");
	}
*/
}
