/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Config.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: bfranco <bfranco@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/09 15:17:36 by bfranco       #+#    #+#                 */
/*   Updated: 2023/11/10 19:57:02 by cariencaljo   ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

Config::Config() : _filename("default.conf")
{
	_readServerSettings();
}

Config::Config(std::string configFile) : _filename(configFile)
{
	_readServerSettings();
}

Config::~Config() {}

std::list<struct ServerSettings>	Config::getServers() const {	return (_servers);	}

void								Config::setFile(std::string filename) {
	_filename = filename;
	_readServerSettings();
}

void	Config::_parseConfigFile()
{
	return ;
}

void	Config::_readServerSettings()
{
	if (_filename == "default.conf")
	{
		struct ServerSettings	server;
		struct LocationSettings	html;

		server._serverName = "Codam_Webserv";
		server._port = 8080;
		server._rootFolder = "./data/html";
		server._index = "index.html";
		
		html._locationName = "/html";
		html._rootFolder = "./data/html";
		html._allowedMethods.push_back("GET");
		html._allowedMethods.push_back("POST");
		html._allowedMethods.push_back("DELETE");
		html._index = "index.html";

		server._locations.push_back(html); //extra locations ???
		_servers.push_back(server);
		
		// ****** test
		struct ServerSettings	server2;
		server2._serverName = "Codam_Webserv2";
		server2._port = 4242;
		server._rootFolder = "./data/html";
		server._index = "index.html";
		// server2._locations.push_back(html);
		_servers.push_back(server2);
		// end test ********
	}
	else
	{
		try {
			_parseConfigFile();
		}
		catch (std::exception &e) {
			std::cout << e.what() << std::endl;
		}
	}

}