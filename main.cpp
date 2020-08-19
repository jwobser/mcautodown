#include <regex>
#include <iostream>
#include <string>
#include <string_view>
#include <filesystem>
#include <fstream>
#include <cassert>
#include <unistd.h>

// N0la rcon lib - https://github.com/n0la/rcon
// #include "rcon/rcon.h"

// Discord Library - https://github.com/zeroxs/aegis.cpp
// #include <aegis.hpp>

// TOML Parsing Lib
#include "tomlplusplus/toml.hpp"

// TODO: Add timestamping for output lines

using std::regex;
using std::string;
using std::string_view;
using std::cout;
using std::smatch;
using std::filesystem::exists;
using std::filesystem::remove;
using namespace std::literals;

// const int TIMEOUT {5};
const std::filesystem::path confpath {"mcdown.yaml"};
const string cp = "mcdown.toml";

int main(int argc, char *argv[]){
	// Read RCON configuration file
	auto config = toml::parse_file(cp);
	// string_view serveraddr = config["rcon"]["addr"].value_or(""sv);
	bool serverempty = config["server"]["empty"].value_or(false);
	// cout << "Server addr: " << serveraddr << '\n';
	// cout << "Server empty flag: " << serverempty << "\n";

	// TODO: Open RCON connection
	// TODO: Get number of players

	// Regex number of players
	/* Example Response:
		There are 0 of a max of 20 players online:
	*/
	string response { R"(There are 0 of a max of 20 players online:)" };
	int playercount = 0;
	regex pat{ R"((?:There are )([0-9]+))" };
	smatch players;
	regex_search(response, players, pat);
	playercount = stoi(players[1]);
	// TODO: Sent output to log file
	cout << "There are: " << players[1] << " players online\n";

	// DELETE ME! : TESTING OUTPUT 
	/*
	if (serverempty) {
		cout << "Server has been empty for " << TIMEOUT << " minutes. Initiating shutdown.\n";
	} else {
		cout << "Server is active.\n";
	}
	*/

	if (playercount == 0 && serverempty){
		cout << "Server is empty. Shutting down.\n";
		try{
			system("docker stop mc_mc_1 >/dev/null 2>/dev/null");
		// System doesn't except when command fails. Maybe get return value?
		} catch(...) {
			cout << "Docker could not stop mc server\n";
		}
		// TODO: Re-enable for production
		// TODO: Possible to use IFDEF flags for dev build vs prod?
		/* system("shutdown -h 60");
		cout << '\n';
		*/

		// TODO: notify discord
	} 

	std::fstream configfile;
	string buffer;
	configfile.open(cp);
	while(!configfile.eof()){
		getline(configfile, buffer);
		if(buffer.length()<5) continue;
		string key = buffer.substr(0,5);
		if(key.compare("empty") == 0){
			// go to beginning of line
			configfile.seekg(-(1+buffer.length()),std::ios_base::cur);

			/* Only change flag to empty if server runnign and empty
			 * Otherwise, reset to false. that way server will not turn
			 * off immediately at next reboot
			 */
			if (!serverempty && playercount == 0){
				configfile << "empty = true \n";
			} else {
				configfile << "empty = false\n";
			}
		}
	}
	configfile.close();
}
