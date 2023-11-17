/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.cpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: cariencaljouw <cariencaljouw@student.co      +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/03 11:11:47 by cariencaljo   #+#    #+#                 */
/*   Updated: 2023/11/03 17:32:38 by cariencaljo   ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "webServ.hpp"
#include <Server.hpp>
#include "Config.hpp"
#include <map>
#include <set>
#include <algorithm>

// ============= con-/destructors ================

Server::Server() {}

Server::Server( Server const & src )  { (void)src; }

Server::~Server() {} // {	close(_fd); }

// ============= Methods ================

int	Server::assign_name()
{
	if (bind(_fd, reinterpret_cast<struct sockaddr*>(&_serverAddr), sizeof(_serverAddr)) == -1) { // check adress
		return 1;
	}
	return 0;
}

int	Server::set_to_listen(int backlog)
{
	if (listen(_fd, backlog) == -1) {
		return 1;
	}
	return 0;
}

int Server::initServer(struct ServerSettings const & settings, int epollFd)
{
	try {
		_settings.push_back(settings);
		_serverAddr.sin_family = AF_INET;
		_serverAddr.sin_addr.s_addr = INADDR_ANY;
		_serverAddr.sin_port = htons(settings._port);
	
		if ((_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK , 0)) == -1) 
			throw ServerException(settings._serverName + " socket: ");
		if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &_serverAddr.sin_addr, sizeof(_serverAddr.sin_addr)))
			throw ServerException(settings._serverName + " setsocket: ");
		if (assign_name())
			throw ServerException(settings._serverName + " bind: ");
		if (set_to_listen(5))
			throw ServerException(settings._serverName + " listen: ");
		if (register_server(epollFd, this->_fd, this))
			throw ServerException(settings._serverName + " register: ");
	}
	catch (const ServerException& e) {
		std::cerr << "\033[31;1mError\n" << e.what() << "\033[0m" << std::endl;
		return 1;
	}
	std::cout << "\033[32;1mServer: " << settings._serverName << ", listening on port "  << settings._port << "\033[0m" << std::endl;
	return 0;
}

void	Server::addClientId(std::string newCookieValue) {
	_knownClientIds.insert(std::make_pair(newCookieValue, 1));
}

int	Server::checkClientId(std::string id) {

	for (auto& pair : _knownClientIds)
	{
		if (pair.first == id)
		{
			pair.second += 1;
			std::cout << "user_id :"<< id << " has visited us " << pair.second << " times!" << std::endl; //for testing
			return  1;
		}
	}
	return 0;
}

void	Server::addSubDomain(struct ServerSettings const & settings) {	_settings.push_back(settings);	}

// ============= Getters ================
uint16_t	Server::get_port(void) const {	return	_settings.front()._port;	}

int	Server::get_FD() const { return _fd; }

std::string	Server::get_serverName(std::string host) const { 
	const ServerSettings *hostSettings = &_settings.front();
	for (auto& setting : _settings) {
		if (setting._serverName == host)
			hostSettings = &setting;
	}
	return hostSettings->_serverName;
}

std::string	Server::get_rootFolder(std::string host) const { 
	const ServerSettings *hostSettings = &_settings.front();
	for (auto& setting : _settings) {
		if (setting._serverName == host)
			hostSettings = &setting;
	}
	return hostSettings->_rootFolder;
}

std::string	Server::get_index(std::string host) const {
	const ServerSettings *hostSettings = &_settings.front();
	for (auto& setting : _settings) {
		if (setting._serverName == host)
			hostSettings = &setting;
	}
	return hostSettings->_index;
}

const struct LocationSettings*	Server::get_locationSettings(std::string host, std::string location) const {
	const ServerSettings *hostSettings = &_settings.front();
	for (auto& setting : _settings) {
		if (setting._serverName == host)
			hostSettings = &setting;
	}
	while (!location.empty())
	{
		for (auto& loc : hostSettings->_locations) {
			if (loc._locationId == location) 
				return (&loc);
		}
		size_t pos = location.rfind('/');
		if (pos == 0 || pos > location.length())
			location = "/";
		else
			location.resize(pos);
	}	
	std::cerr << RED << "Error\nincorrect configuration " << hostSettings->_serverName << RESET << std::endl;
	return (nullptr);
}

std::map<std::string, int>	Server::get_knownClientIds() const {	return _knownClientIds;	}
	
double	Server::get_timeout(std::string host) const { 
	const ServerSettings *hostSettings = &_settings.front();
	for (auto& setting : _settings) {
		if (setting._serverName == host)
			hostSettings = &setting;
	}
	return hostSettings->_timeout; 
}

int		Server::get_maxNrOfRequests(std::string host) const { 
	const ServerSettings *hostSettings = &_settings.front();
	for (auto& setting : _settings) {
		if (setting._serverName == host)
			hostSettings = &setting;
	}
	return hostSettings->_maxNrOfRequests; 
}

size_t	Server::get_maxBodySize(std::string host) const {
	const ServerSettings *hostSettings = &_settings.front();
	for (auto& setting : _settings) {
		if (setting._serverName == host)
			hostSettings = &setting;
	}
	return hostSettings->_maxBodySize; 
}

// std::list<ErrorPages>	Server::get_errorPages() const { return _errorPages; }

/* ************************************************************************** */

std::list<Server> initServers(std::list<struct ServerSettings> settings, int epollFd)
{
	std::list<Server> 				servers;
	std::map<uint16_t, std::string> serverMap;
	std::set<uint16_t>				ports;

	try {
		for (auto& setting : settings)
		{
			std::set<uint16_t>::iterator p = ports.find(setting._port);
			if (p == ports.end()) {
				ports.insert(setting._port);
				serverMap.insert(std::make_pair(setting._port, setting._serverName));
				std::cout << setting._port << ": added to set" << std::endl;
				servers.push_back(Server());
				if (servers.back().initServer(setting, epollFd) == 1)
					servers.pop_back();
			}
			else {
				for (auto& server : servers) {
					for (auto& pair : serverMap) {
						if (pair.first == setting._port && pair.second == setting._serverName)
							throw std::runtime_error(setting._serverName + " allready configured for this port");
					}
					if (server.get_port() == setting._port) {
						server.addSubDomain(setting);
						std::cout << GREEN << setting._serverName << ", listening on port "  \
															<< setting._port << RESET << std::endl;
						break ;
					}
				}
			}
		}
	}
	catch (const std::runtime_error& e) {
		std::cerr << RED << "Error\n" << e.what() << RESET << std::endl;
	}
	return (servers);
}
