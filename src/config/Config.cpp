/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Config.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: bfranco <bfranco@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/09 15:17:36 by bfranco       #+#    #+#                 */
/*   Updated: 2023/11/10 19:00:36 by bfranco       ########   odam.nl         */
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
	}
	else
	{
		if (access(_filename.c_str(), R_OK) == -1)
			throw NoSuchFileException();
		if (_parseConfigFile() == -1)
			throw InvalidParameterException();
	}
}