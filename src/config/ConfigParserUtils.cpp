/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ConfigParserUtils.cpp                              :+:    :+:            */
/*                                                     +:+                    */
/*   By: bfranco <bfranco@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/26 18:20:33 by bfranco       #+#    #+#                 */
/*   Updated: 2023/11/27 12:19:20 by bfranco       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

int	parseServer(std::string line, struct ServerSettings *server)
{
	std::string key = line.substr(0, line.find_first_of(" "));
	std::string value = line.substr(line.find_first_of(" ") + 1);

	std::cout << "key: " << key << std::endl;
	std::cout << "value: " << value << std::endl;
	if (key.empty() || value.empty())
		return 1;
	
	if (key == "server_name")
		server->_serverName = value;
	else if (key == "root")
		server->_rootFolder = value;
	else if (key == "listen")
	{
		if (value.length() >= 10 || value.find_first_not_of("0123456789") != std::string::npos)
			return 1;
		server->_port = std::stol(value);
	}
	else if (key == "index")
		server->_index = value;
	else if (key == "timeout")
	{
		if (value.length() >= 10 || value.find_first_not_of("0123456789") != std::string::npos)
			return 1;
		server->_timeout = std::stol(value);
	}
	else if (key == "max_requests")
	{
		if (value.length() >= 10 || value.find_first_not_of("0123456789") != std::string::npos)
			return 1;
		server->_maxNrOfRequests = std::stol(value);
	}
	else if (key == "client_max_body_size")
	{
		if (value.length() >= 10 || value.find_first_not_of("0123456789") != std::string::npos)
			return 1;
		server->_maxBodySize = std::stol(value);
	}
	else if (key == "upload_dir")
		server->_uploadDir = value;
	else
		return 1;
	return 0;
}

int	parseLocation(std::string line, struct LocationSettings *location)
{
	std::string key = line.substr(0, line.find_first_of(" "));
	std::string value = line.substr(line.find_first_of(" ") + 1);
	
	std::cout << "key: " << key << std::endl;
	std::cout << "value: " << value << std::endl;
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
		location->_index = value;
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
		std::string code = value.substr(0, value.find_first_of(" "));
		std::string uri = value.substr(value.find_first_of(" ") + 1);
		
		if (code.empty() || uri.empty() || uri[0] != '/')
			return 1;
		if (code.length() != 3 || code.find_first_not_of("0123456789") != std::string::npos)
			return 1;

		int errcode = std::stoi(code);
		location->_redirect[errcode] = uri;
	}
	else
		return 1;
	return 0;
}

int	parseErrorPage(std::string line, std::map<int, std::string> *errorPages)
{
	std::string key = line.substr(0, line.find_first_of(" "));
	std::string value = line.substr(line.find_first_of(" ") + 1);
	
	std::cout << "key: " << key << std::endl;
	std::cout << "value: " << value << std::endl;
	if (key.empty() || value.empty() || key.find_first_of(" \t") != std::string::npos)
		return 1;
	if (key.length() != 3 || key.find_first_not_of("0123456789") != std::string::npos || value[0] != '/')
		return 1;

	int code = std::stoi(key);
	(*errorPages)[code] = value;
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

	size_t	start = 9;
	size_t	end = line.find(" {", 9);
	static_cast<struct LocationSettings *>(location)->_locationId = line.substr(start, end);

	return location;
}
