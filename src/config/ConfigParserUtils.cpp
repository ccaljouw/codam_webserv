/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ConfigParserUtils.cpp                              :+:    :+:            */
/*                                                     +:+                    */
/*   By: bfranco <bfranco@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/26 18:20:33 by bfranco       #+#    #+#                 */
/*   Updated: 2023/11/27 17:39:56 by bfranco       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include <unistd.h>

int	parseServer(std::string line, struct ServerSettings *server)
{
	std::string key = line.substr(0, line.find_first_of(WHITESPACE));
	std::string value = line.substr(line.find_first_of(WHITESPACE) + 1);

	// std::cout << "key: " << key << std::endl;
	// std::cout << "value: " << value << std::endl;
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
		if (access(value.c_str(), W_OK) == -1)
			return 1;
		server->_rootFolder = value;
	}
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
	
	// std::cout << "key: " << key << std::endl;
	// std::cout << "value: " << value << std::endl;
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
	else if (key == "autoindex")
	{
		if (value == "on")
			location->_autoindex = true;
		else if (value == "off")
			location->_autoindex = false;
		else
			return 1;
	}
	else if (key == "return")
	{
		std::string code = value.substr(0, value.find_first_of(WHITESPACE));
		std::string uri = value.substr(value.find_first_of(WHITESPACE) + 1);
		
		if (code.empty() || uri.empty() || uri[0] != '/')
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
	
	// std::cout << "key: " << key << std::endl;
	// std::cout << "value: " << value << std::endl;
	if (key.empty() || value.empty() || key.find_first_of(" \t") != std::string::npos)
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
	void	*server = new struct ServerSettings();

	static_cast<struct ServerSettings *>(server)->_locations = std::list<struct LocationSettings*>();
	static_cast<struct ServerSettings *>(server)->_errorPages = nullptr;

	return server;
}

void	*initLocationBlock(std::string line)
{
	void	*location = new struct LocationSettings();

	line = line.substr(9);
	size_t	start = 0;
	size_t	end = line.find_first_of(" \t\n\v\f\r{");
	static_cast<struct LocationSettings *>(location)->_locationId = line.substr(start, end);

	return location;
}

// void checkMandatoryParameters