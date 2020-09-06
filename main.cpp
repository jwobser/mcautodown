// #define DEBUG 1
#include <regex>
#include <iostream>
#include <string>
#include <string_view>
#include <filesystem>
#include <fstream>
#include <cassert>
#include <unistd.h>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/regex.hpp>
#include <boost/thread.hpp>
#include <chrono>



// TOML Parsing Lib
#include "tomlplusplus/toml.hpp"

// TODO: Add timestamping for output lines
// TODO: Open RCON connection

// using boost::regex;
using std::regex;
using std::string;
using std::string_view;
using std::cout;
using std::smatch;
// using boost::smatch;
using std::filesystem::exists;
using std::filesystem::remove;
using namespace std::literals;
using namespace boost::asio::ip;
// const int TIMEOUT {5};
const std::filesystem::path confpath {"mcdown.toml"};
const string cp = "mcdown.toml";
const int MC_RCON_PORT = 25575;

class rconpacket {
	public:
	rconpacket() :size{0}, ID{0}, type{0}, body{""} {};
	rconpacket(int size, int ID, int type, string command);

	private:
	int32_t size = 0;
	int32_t ID = 0;
	int32_t type = 0;
	string body;
};

int main(int argc, char *argv[]){
	boost::asio::io_context io_context;
	// Read RCON configuration file
	auto config = toml::parse_file(cp);

	// Get Values
	string serveraddr = string(config["rcon"]["addr"].value_or(""sv));
	string serverpw = string(config["rcon"]["passwd"].value_or(""sv));
	const int serverport = config["rcon"]["port"].value_or(MC_RCON_PORT);
	bool serverempty = config["server"]["empty"].value_or(false);
	cout << "Server addr: " << serveraddr << '\n';
	cout << "Server empty flag: " << serverempty << "\n";

	tcp::socket mysock(io_context);
	tcp::endpoint server = tcp::endpoint(address::from_string(serveraddr), 25575);
	// mysock.connect(tcp::endpoint(server));
	mysock.open(boost::asio::ip::tcp::v4());
	cout << "Opened \n";
	boost::asio::ip::tcp::socket::reuse_address option(true);
	mysock.set_option(option);
	cout << "Set Reuse address option \n";
	mysock.bind(server);
	cout << "Bound \n";
	mysock.send(boost::asio::buffer("test"));
	cout << "Sent \n";
	// boost::asio::connect(mysock, server);
	string command{"test"};
	int packlen = 10 + command.length();
	cout << "Message length: " << packlen << std::endl;

	cout << "Waiting for 1s." << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	cout << "Finished Waiting." << std::endl;
	



	// mysock.receive();
	// mysock.open(boost::asio::ip::tcp::v4());

	// cout << "Attempted to get RCON connection \n";
	// string discard;
	// std::cin >> discard;

	// Regex number of players
	/* Example Response:
		There are 0 of a max of 20 players online:
	*/
	string response { R"(There are 0 of a max of 20 players online:)" };
	cout << "Response: " << response << '\n';
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
	// If empty key is missing, server will never shutdown
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
