/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Config.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: bfranco <bfranco@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/09 15:17:36 by bfranco       #+#    #+#                 */
/*   Updated: 2023/11/26 20:53:05 by bfranco       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include <fstream>
#include <map>
#include <vector>

Config::Config(int argc, char** argv) : _servers(), _error(false), _lineNr(1)
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
		std::cerr << "Line " << _lineNr << ":" << e.what() << std::endl;
	}
}

Config::~Config()
{
	for (auto& server : _servers) {
		for (auto& location : server->_locations) {
			delete location;
		}

		server->_errorPages->clear();
		server->_locations.clear();
		delete server;
	}
	_servers.clear();
}

std::list<struct ServerSettings*>	Config::getServers() const	{	return (_servers);	}
bool								Config::getError() const	{	return (_error);	}

void	Config::_readConfigFile()
{
	// if (_filename == "default.conf")
	// {
	// 	struct ServerSettings	server;
	// 	struct LocationSettings	html;
	// 	struct LocationSettings	html2;

	// 	server._serverName = "Codam_Webserv";
	// 	server._port = 8080;
	// 	server._rootFolder = "./data/html";
	// 	server._index = "index.html";
	// 	server._timeout = 3;
	// 	server._maxNrOfRequests = 10;
	// 	server._maxBodySize =  2 * 1024 * 1024; // 2 MB in bytes
		
	// 	html._locationId = "/";
	// 	html._allowedMethods.insert("POST");
	// 	html._allowedMethods.insert("DELETE");
	// 	html._allowedMethods.insert("GET");
	// 	html._index = "index.html";
	// 	server._locations.push_back(html); //extra locations ???

	// 	html2._locationId = "/index.html";
	// 	html2._allowedMethods.insert("POST");
	// 	html2._allowedMethods.insert("DELETE");
	// 	html2._allowedMethods.insert("GET");
	// 	html2._index = "index.html";
	// 	server._locations.push_back(html2);
		
	// 	_servers.push_back(server);
		
	// 	// ****** test
	// 	struct ServerSettings	server2;
	// 	server2._serverName = "Codam_Webserv2";
	// 	server2._port = 4242;
	// 	server2._rootFolder = "./data_website2/html";
	// 	server2._index = "index.html";
	// 	server2._locations.push_back(html);
	// 	_servers.push_back(server2);
	// 	// end test ********
	// }
	// else
	// {
		std::ifstream	configFile(_filename.c_str());
		std::string		line;
		configBlock		currentBlock = NONE;
		void			*currentBlockPtr = NULL;

		// check if file is valid and open
		if (!configFile.is_open())
			throw NoSuchFileException();

		// read file line by line
		while (std::getline(configFile, line))
		{
			line.erase(0, line.find_first_not_of(" \t"));
			line.erase(line.find_last_not_of("; \t") + 1);
			// std::cout << "|" << line << "|" << std::endl;
			if (line.empty() || line[0] == '#')
			{
				_lineNr++;
				continue;
			}

			if (currentBlock == NONE && line.find("server {") != std::string::npos)
			{
				currentBlock = SERVER;
				currentBlockPtr = new struct ServerSettings();
			}
			else if (currentBlock == SERVER && line.find("location /") != std::string::npos && line.find(" {") != std::string::npos)
			{
				currentBlock = LOCATION;
				currentBlockPtr = new struct LocationSettings();
				size_t start = 9;
				size_t end = line.find(" {", 9);
				static_cast<struct LocationSettings *>(currentBlockPtr)->_locationId = line.substr(start, end);
			}
			else if (currentBlock == SERVER && line.find("errorpages {") != std::string::npos)
			{
				currentBlock = ERROR_PAGE;
				currentBlockPtr = new std::map<int, std::string>;
			}
			else if (line.find("}") != std::string::npos)
			{
				if (_handleBlockEnd(&currentBlock, currentBlockPtr) == 1)
					throw InvalidParameterException();
			}
			else
			{
				if (_handleBlockContent(line, currentBlock, currentBlockPtr) == 1)
					throw InvalidParameterException();
			}
			_lineNr++;
		}
	// }
}

int	Config::_handleBlockEnd(configBlock *currentBlock, void *currentBlockPtr)
{
	switch (*currentBlock) {
		case SERVER:
			_servers.push_back(static_cast<struct ServerSettings *>(currentBlockPtr));
			*currentBlock = NONE;
			break;
		case LOCATION:
			_servers.back()->_locations.push_back(static_cast<struct LocationSettings *>(currentBlockPtr));
			*currentBlock = SERVER;
			break;
		case ERROR_PAGE:
			_servers.back()->_errorPages = static_cast<std::map<int, std::string>*>(currentBlockPtr);
			std::cout << "errorpages end " << std::endl;
			*currentBlock = SERVER;
			break;
		default:
			return 1;
	}
	return 0;
}

int	Config::_handleBlockContent(std::string line, configBlock currentBlock, void *currentBlockPtr)
{
	switch (currentBlock) {
		case SERVER:
			return parseServer(line, static_cast<struct ServerSettings *>(currentBlockPtr));
		case LOCATION:
			return parseLocation(line, static_cast<struct LocationSettings *>(currentBlockPtr));
		case ERROR_PAGE:
			return parseErrorPage(line, static_cast<std::map<int, std::string> *>(currentBlockPtr));
		default:
			return 1;
	}
	return 0;
}