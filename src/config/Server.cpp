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

Server::Server() : _fd(0), _conn(nullptr) {
	// std::cout << CYAN << "server default constructor called" << RESET << std::endl;
}

Server::Server( Server const & src ) : _conn(nullptr) { 
	_fd = src.get_FD();
	_timeout = src.get_timeout();
	_maxNrOfRequests = src.get_maxNrOfRequests();
	// add other stuff...
	// std::cout << CYAN << "server copy constructor called" << RESET << std::endl;
}

Server::~Server() {	
	if (_fd) {
		std::cout << CYAN << "server destructor called for fd: " << _fd << RESET << std::endl;
		close(_fd); 
	}
	if (_conn) {
		std::cout << CYAN << "connection struct deleted" << RESET << std::endl;
		delete _conn;
	}
}

// ============= Methods ================

int	Server::assign_name()
{
	int enable = 1;
	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)))
		return 1;
	if (bind(_fd, reinterpret_cast<struct sockaddr*>(&_serverAddr), sizeof(_serverAddr)) == -1) {
		return 1;
	}
		// throw ServerException(settings._serverName + " setsocket: ");
	return 0;
}

int	Server::set_to_listen(int backlog)
{
	if (listen(_fd, backlog) == -1) {
		return 1;
	}
	return 0;
}

int Server::initServer(struct ServerSettings *settings, int epollFd, double timeout, int maxNrOfRequests)
{
	try {
		_timeout = timeout,
		_maxNrOfRequests = maxNrOfRequests;
		_settings.push_back(settings);
		std::cout << CYAN << _settings.front()->_serverName << RESET << std::endl;
		_serverAddr.sin_family = AF_INET;
		_serverAddr.sin_addr.s_addr = INADDR_ANY;
		_serverAddr.sin_port = htons(settings->_port);
	
		if ((_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK , 0)) == -1) 
			throw ServerException(settings->_serverName + " socket: ");
		if (assign_name())
			throw ServerException(settings->_serverName + " bind: ");
		if (set_to_listen(5))
			throw ServerException(settings->_serverName + " listen: ");
		if (register_server(epollFd, this->_fd, this))
			throw ServerException(settings->_serverName + " register: ");
	}
	catch (const ServerException& e) {
		std::cerr << RED << e.what() << RESET << std::endl;
		return 1;
	}
	std::cout << "\033[32;1mServer: " << settings->_serverName << ", listening on port "  << settings->_port << "\033[0m" << std::endl;
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
			// std::cout << "user_id :"<< id << " has visited us " << pair.second << " times!" << std::endl; //for testing
			return  1;
		}
	}
	return 0;
}

void	Server::addSubDomain(struct ServerSettings *settings) {	_settings.push_back(settings);	}

// ============= Getters ================
uint16_t					Server::get_port(void) const {	return	_settings.front()->_port;	}
int							Server::get_FD() const { return _fd; }
std::map<std::string, int>	Server::get_knownClientIds() const {	return _knownClientIds;	}	
double						Server::get_timeout() const { return _timeout; }
int							Server::get_maxNrOfRequests() const { return _maxNrOfRequests; }

std::string	Server::get_serverName(std::string host) const { 
	const ServerSettings *hostSettings = _settings.front();
	for (auto& setting : _settings) {
		if (setting->_serverName == host)
			hostSettings = setting;
	}
	return hostSettings->_serverName;
}

std::string	Server::get_rootFolder(std::string host) const { 
	const ServerSettings *hostSettings = _settings.front();
	for (auto& setting : _settings) {
		if (setting->_serverName == host)
			hostSettings = setting;
	}
	return hostSettings->_rootFolder;
}

std::string	Server::get_index(std::string host) const {
	const ServerSettings *hostSettings = _settings.front();
	for (auto& setting : _settings) {
		if (setting->_serverName == host)
			hostSettings = setting;
	}
	return hostSettings->_index;
}

const struct LocationSettings*	Server::get_locationSettings(std::string host, std::string location) const {
	
	struct ServerSettings *hostSettings = _settings.front();
	for (auto& setting : _settings) {
		if (setting->_serverName == host)
			hostSettings = setting;
	}
	// check for non existing hostnames? of just go with an available host on the same port?
	while (!location.empty())
	{
		// std::cout << "looking for " << location << " in " << std::endl;
		for (auto& loc : hostSettings->_locations) {
			// std::cout << "now " << loc->_locationId << std::endl;
			if (loc->_locationId == location) 
				return (loc);
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

size_t	Server::get_maxBodySize(std::string host) const {
	const ServerSettings *hostSettings = _settings.front();
	for (auto& setting : _settings) {
		if (setting->_serverName == host)
			hostSettings = setting;
	}
	return hostSettings->_maxBodySize; 
}

struct connection *Server::get_connection() {	return _conn;	}

// std::list<ErrorPages>	Server::get_errorPages() const { return _errorPages; }

void	Server::set_connection(struct connection *conn) {
	_conn = conn;
}

/* ************************************************************************** */

std::list<Server> initServers(const Config& config, int epollFd)
{
	std::list<Server> 					servers;
	std::map<uint16_t, std::string> 	serverMap;
	std::set<uint16_t>					ports;
	std::list<struct ServerSettings *>	settings = config.getServers();

	try {
		for (auto setting : settings)
		{
			std::set<uint16_t>::iterator p = ports.find(setting->_port);
			if (p == ports.end()) {
				ports.insert(setting->_port);
				serverMap.insert(std::make_pair(setting->_port, setting->_serverName));
				servers.push_back(Server());
				if (servers.back().initServer(setting, epollFd, TIMEOUT, MAX_NR_REQUESTS) == 1)
					servers.pop_back();
				(void)epollFd;
			}
			else {
				for (auto& server : servers) {
					for (auto& pair : serverMap) {
						if (pair.first == setting->_port && pair.second == setting->_serverName)
							throw std::runtime_error(setting->_serverName + ":" + std::to_string(setting->_port) + " allready configured");
					}
					if (server.get_port() == setting->_port) {
						server.addSubDomain(setting);
						std::cout << GREEN << setting->_serverName << ", listening on port "  \
															<< setting->_port << RESET << std::endl;
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
