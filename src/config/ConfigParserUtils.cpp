/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ConfigParserUtils.cpp                              :+:    :+:            */
/*                                                     +:+                    */
/*   By: ccaljouw <ccaljouw@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/26 18:20:33 by bfranco       #+#    #+#                 */
/*   Updated: 2023/11/28 11:57:27 by cariencaljo   ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include <unistd.h>
#include <string.h>
#include <string>

int	parseServer(std::string line, struct ServerSettings *server)
{
	std::string key = line.substr(0, line.find_first_of(WHITESPACE));
	std::string value = line.substr(line.find_first_of(WHITESPACE) + 1);

	if (key.empty() || value.empty())
		return 1;
	
	if (key == "server_name")
	{
		if (value.find_first_not_of(std::string(LETTERS) + std::string(NUMBERS) + std::string("_")) != std::string::npos)
			return 1;
		server->_serverName = value;
	}
	else if (key == "root")
	{
		if (access(value.c_str(), F_OK | R_OK) == -1) 
			return 1;
		server->_rootFolder = value;
	}
	else if (key == "upload_dir")
		server->_uploadDir = value;
	else if (key == "listen")
	{
		if (value.length() >= 10 || value.find_first_not_of(NUMBERS) != std::string::npos)
			return 1;
		try { server->_port = std::stol(value); }
		catch (const std::exception& e)
		{ return 1; }
	}
	else if (key == "index")
	{
		if (value.find_first_not_of(std::string(LETTERS) + std::string(NUMBERS) + std::string("_/.")) != std::string::npos)
			return 1;
		server->_index = value;
	}
	else if (key == "client_max_body_size")
	{
		if (value.length() >= 10 || value.find_first_not_of(NUMBERS) != std::string::npos)
			return 1;
		try { server->_maxBodySize = std::stol(value); }
		catch (const std::exception& e)
		{ return 1; }
	}
	else
		return 1;
	return 0;
}

int	parseLocation(std::string line, struct LocationSettings *location)
{
	std::string key = line.substr(0, line.find_first_of(WHITESPACE));
	std::string value = line.substr(line.find_first_of(WHITESPACE) + 1);

	if (key.empty() || value.empty())
		return 1;
	if (key == "allow")
	{
		if (value == "GET" || value == "POST" || value == "DELETE")
			location->_allowedMethods.insert(value);
		else
			return 1;
	}
	else if (key == "index")
	{
		if (value.find_first_not_of(std::string(LETTERS) + std::string(NUMBERS) + std::string("_/.")) != std::string::npos)
			return 1;
		location->_index = value;
	}
	else if (key == "dirListing")
	{
		if (value == "on")
			location->_dirListing = true;
		else if (value == "off")
			location->_dirListing = false;
		else
			return 1;
	}
	else if (key == "return")
	{
		std::string code = value.substr(0, value.find_first_of(WHITESPACE));
		std::string uri = value.substr(value.find_first_of(WHITESPACE) + 1);
		
		if (code.empty() || uri.empty() || uri.find_first_of(WHITESPACE) != std::string::npos)
			return 1;
		if (code.length() != 3 || code.find_first_not_of(NUMBERS) != std::string::npos)
			return 1;

		try {
			int errcode = std::stoi(code);
			location->_redirect[errcode] = uri;
		}
		catch (const std::exception& e)
		{ return 1; }
	}
	else
		return 1;
	return 0;
}

int	parseErrorPage(std::string line, std::map<int, std::string> *errorPages)
{
	std::string key = line.substr(0, line.find_first_of(WHITESPACE));
	std::string value = line.substr(line.find_first_of(WHITESPACE) + 1);

	if (key.empty() || value.empty() || value.find_first_of(WHITESPACE) != std::string::npos)
		return 1;
	if (key.length() != 3 || key.find_first_not_of(NUMBERS) != std::string::npos || value[0] != '/')
		return 1;
	
	try {
		int code = std::stoi(key);
		(*errorPages)[code] = value;
	}
	catch (const std::exception& e)
	{ return 1; }

	return 0;
}

void	*initServerBlock()
{
	struct ServerSettings *server = new struct ServerSettings();

	server->_serverName = "";
	server->_rootFolder = "";
	server->_uploadDir = "";
	server->_index = "";
	server->_port = 0;
	server->_maxBodySize = 0;
	server->_errorPages = nullptr;
	server->_locations = std::list<struct LocationSettings*>();

	return static_cast<void *>(server);
}

void	*initLocationBlock(std::string line)
{
	struct LocationSettings	*location = new struct LocationSettings();

	line = line.substr(9);
	location->_locationId = line.substr(0, line.find_first_of(" \t\n\v\f\r{"));
	location->_allowedMethods = std::set<std::string>();
	location->_redirect = std::map<int, std::string>();
	location->_index = "";
	location->_dirListing = false;

	return static_cast<void *>(location);
}

void	deleteBlock(const configBlock& currentBlock, void *currentBlockPtr)
{
	if (currentBlock == LOCATION)
	{
		struct LocationSettings *location = static_cast<struct LocationSettings *>(currentBlockPtr);
		delete location;
	}
	else if (currentBlock == ERROR_PAGE)
	{
		std::map<int, std::string> *errorPages = static_cast<std::map<int, std::string> *>(currentBlockPtr);
		delete errorPages;
	}
	currentBlockPtr = nullptr;
}