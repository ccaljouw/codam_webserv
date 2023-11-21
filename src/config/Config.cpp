/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Config.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: bfranco <bfranco@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/09 15:17:36 by bfranco       #+#    #+#                 */
/*   Updated: 2023/11/21 09:11:32 by carlo         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include <fstream>
#include <map>
#include <vector>

Config::Config(int argc, char** argv) : _error(false)
{
	if (argc == 1)
		_filename = std::string("default.conf");
	else if (argc >= 2)
	{
		_filename = std::string(argv[1]);
		if (argc > 2)
		{
			std::cerr << "Too many arguments... Ignoring all but first" << std::endl;
		}
	}
	try
	{
		_readConfigFile();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}

Config::~Config() {}

std::list<struct ServerSettings>	Config::getServers() const	{	return (_servers);	}
bool								Config::getError() const	{	return (_error);	}

void	Config::_readConfigFile()
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
		
		html._locationId = "/";
		html._allowedMethods.insert("POST");
		html._allowedMethods.insert("DELETE");
		html._allowedMethods.insert("GET");
		html._index = "index.html";
		server._locations.push_back(html); //extra locations ???

		html2._locationId = "/index.html";
		html2._allowedMethods.insert("POST");
		html2._allowedMethods.insert("DELETE");
		html2._allowedMethods.insert("GET");
		html2._index = "index.html";
		server._locations.push_back(html2);
		
		_servers.push_back(server);
		
		// ****** test
		struct ServerSettings	server2;
		server2._serverName = "Codam_Webserv2";
		server2._port = 4242;
		server2._rootFolder = "./data_website2/html";
		server2._index = "index.html";
		server2._locations.push_back(html);
		_servers.push_back(server2);
		// end test ********
	}
	else
	{
		std::ifstream	configFile(_filename.c_str());
		std::string		line;
		unsigned int	lineNumber = 0;

		// check if file is valid and open
		if (!configFile.is_open())
			throw NoSuchFileException();

		// read file line by line
		while (std::getline(configFile, line, static_cast<char>(std::cout.eof()))) {
			try {
				_parseConfigFile(line);
				lineNumber++;
			}
			catch(const std::exception& e) {
				std::cerr << "Error in line " << lineNumber << ": " << e.what() << std::endl;
			}
		}
	}
}

void	Config::_parseConfigFile(const std::string& line)
{
	// std::map<std::string> blocks = {{"server"}, "location", "errorpages"};
	size_t	start = line.find_first_not_of(" \t\n\r\f\v");
	size_t	end = line.find_last_not_of(" \t\n\r\f\v");

	if (start == std::string::npos || line[start] == '#')
		return ;
	std::cout << _filename << std::endl;
	std::string	trimmedLine = line.substr(start, end - start + 1);
	if (trimmedLine.empty())
		return ;
	std::cout << trimmedLine << std::endl;
	std::vector< std::string > splitLine;
	start = 0;
	end = trimmedLine.find_first_of(" \0");
	while (start != std::string::npos)
	{
		splitLine.push_back(trimmedLine.substr(start, end - start + 1));
		start = end + 1;
	}
	// for (auto item : splitLine)
		// std::cout << item << std::endl
}