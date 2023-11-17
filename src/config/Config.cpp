/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Config.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: ccaljouw <ccaljouw@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/09 15:17:36 by bfranco       #+#    #+#                 */
/*   Updated: 2023/11/17 16:30:00 by cariencaljo   ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

Config::Config(int argc, char** argv) : _error(false)
{
	if (argc == 1)
		_filename = std::string("default.conf");
	else if (argc >= 2)
	{
		_filename = std::string(argv[1]);
		if (argc > 2)
		{
			_error = true;
			std::cerr << "Too many arguments... Ignoring all but first" << std::endl;
		}
	}
	try
	{
		_readServerSettings();
	}
	catch(const std::exception& e)
	{
		_error = true;
		std::cerr << e.what() << std::endl;
	}
}

Config::~Config() {}

std::list<struct ServerSettings>	Config::getServers() const	{	return (_servers);	}
bool								Config::getError() const	{	return (_error);	}

int	Config::_parseConfigFile()
{
	return 0;
}

void	Config::_readServerSettings()
{
	if (_filename == "default.conf")
	{
		struct ServerSettings	server;
		struct LocationSettings	html;
		struct LocationSettings	html2;

		server._serverName = "Codam_Webserv";
		server._port = 8080;
		server._rootFolder = "./data/html";
		server._index = "index.html";
		server._timeout = 3;
		server._maxNrOfRequests = 10;
		
		html._locationId = "/html";
		html._allowedMethods.insert("POST");
		html._allowedMethods.insert("DELETE");
		html._allowedMethods.insert("GET");
		html._index = "index.html";
		server._locations.push_back(html); //extra locations ???

		html2._locationId = "/index.html";
		html2._allowedMethods.insert("POST");
		html2._allowedMethods.insert("DELETE");
		// html2._allowedMethods.insert("GET");
		html2._index = "index.html";
		server._locations.push_back(html2);
		
		_servers.push_back(server);
		
		// ****** test
		struct ServerSettings	server2;
		server2._serverName = "Codam_Webserv2";
		server2._port = 8080;
		server2._rootFolder = "./data_website2/html";
		server2._index = "index.html";
		server2._locations.push_back(html);
		_servers.push_back(server2);
		// end test ********
	}
	else
	{
		if (access(_filename.c_str(), R_OK) == -1)
			throw NoSuchFileException();
		if (_parseConfigFile() == -1)
			throw InvalidParameterException();
	}
}