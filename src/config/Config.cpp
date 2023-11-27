/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Config.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: ccaljouw <ccaljouw@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/09 15:17:36 by bfranco       #+#    #+#                 */
/*   Updated: 2023/11/29 10:07:24 by carlo         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "webServ.hpp"
#include <fstream>
#include <map>
#include <list>

Config::Config(int argc, char** argv) : _error(false), _lineNr(1)
{
	if (argc == 1) {
		_filename = std::string("./config/default.conf");
		std::cout << GREEN << "default config loading...." << RESET << std::endl;
	}
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
		_servers = std::list<struct ServerSettings*>();
		_readConfigFile();
		for (const auto server : _servers)
			checkMandatoryParameters(server);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	std::cout << "\nnb servers = " << _servers.size()<< std::endl;

	for (const auto server : _servers) {
		std::cout << "Server: " << server->_serverName << std::endl;
		std::cout << server->_index <<  std::endl;
		std::cout << server->_maxBodySize <<  std::endl;
		std::cout << server->_port << std::endl;
		std::cout << server->_rootFolder <<  std::endl;

		std::cout << "location size: " << server->_locations.size()<< std::endl;
		for (const auto location : server->_locations) {
			std::cout << "Location ID: " << location->_locationId << std::endl;
			std::cout << location->_dirListing << std::endl;
			std::cout << location->_index << std::endl;
			// std::cout << location->_redirect[0] << std::endl;
			for (const auto& method : location->_allowedMethods)
				std::cout << method << std::endl;
		}

		// std::cout << server->_errorPages << std::endl;
			
	}

}

Config::~Config()
{
	for (auto& server : _servers) {
		for (auto& location : server->_locations) {
			delete location;
		}
		server->_locations.clear();
		server->_errorPages->clear();
		delete server->_errorPages;
		delete server;
	}
	_servers.clear();
}

std::list<struct ServerSettings*>	Config::getServers() const	{	return (_servers);	}
bool								Config::getError() const	{	return (_error);	}

void	Config::_readConfigFile()
{
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
		line.erase(0, line.find_first_not_of(" \t\n\v\f\r"));
		line.erase(line.find_last_not_of("; \t\n\v\f\r") + 1);
		// std::cout << "|" << line << "|" << std::endl;
		if (line.empty() || line[0] == '#')
		{
			_lineNr++;
			continue;
		}

		if (currentBlock == NONE && line.find("server {") != std::string::npos)
		{
			currentBlock = SERVER;
			currentBlockPtr = initServerBlock();
			_servers.push_back(static_cast<struct ServerSettings *>(currentBlockPtr));
		}
		else if (currentBlock == SERVER && line.find("location /") != std::string::npos && line.find(" {") != std::string::npos)
		{
			currentBlock = LOCATION;
			currentBlockPtr = initLocationBlock(line);
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
}

int	Config::_handleBlockEnd(configBlock *currentBlock, void *currentBlockPtr)
{
	switch (*currentBlock) {
		case SERVER:
			*currentBlock = NONE;
			break;
		case LOCATION:
			_servers.back()->_locations.push_back(static_cast<struct LocationSettings *>(currentBlockPtr));
			*currentBlock = SERVER;
			break;
		case ERROR_PAGE:
			_servers.back()->_errorPages = static_cast<std::map<int, std::string>*>(currentBlockPtr);
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