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

// ============= con-/destructors ================

Server::Server() {}

Server::Server( Server const & src ) { (void)src; }

Server::~Server() {	close(_fd); }

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
		_port = settings._port;
		_serverName = settings._serverName;
		_locations = settings._locations;
		_timeout = settings._timeout;
		_maxNrOfRequests = settings._maxNrOfRequests;
		_serverAddr.sin_family = AF_INET;
		_serverAddr.sin_addr.s_addr = INADDR_ANY;
		_serverAddr.sin_port = htons(_port);
	
		if ((_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK , 0)) == -1) 
			throw ServerException(_serverName + " socket: ");
		if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<struct sockaddr*>(&_serverAddr), sizeof(_serverAddr)))
			throw ServerException(_serverName + " setsocket: ");
		if (assign_name())
			throw ServerException(_serverName + " bind: ");
		if (set_to_listen(5))
			throw ServerException(_serverName + " listen: ");
		if (register_server(epollFd, this->_fd, this))
			throw ServerException(_serverName + " register: ");
	}
	catch (const ServerException& e) {
		std::cerr << "\033[31;1mError\n" << e.what() << "\033[0m" << std::endl;
		return 1;
	}
	std::cout << "\033[32;1mServer: " << settings._serverName << ", listening on port "  << _port << "\033[0m" << std::endl;
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

// ============= Getters ================
int	Server::get_FD(std::string server) const { 
	(void)server;
	return _fd; }

std::string	Server::get_serverName(std::string server) const { 
	(void)server;
	return _serverName; }

std::string	Server::get_rootFolder(std::string server) const { 
	(void)server;
	return _rootFolder; }

std::string	Server::get_index(std::string server) const { 
	(void)server;
	return _index; }

std::list<struct LocationSettings>	Server::get_locations(std::string server) const { 
	(void)server;
	return _locations; }

std::map<std::string, int>	Server::get_knownClientIds(std::string server) const { 
	(void)server;
	return _knownClientIds; }
	
double	Server::get_timeout(std::string server) const { 
	(void)server;
	return _timeout; }

int		Server::get_maxNrOfRequests(std::string server) const { 
	(void)server;
	return _maxNrOfRequests; }

size_t	Server::get_maxBodySize(std::string server) const {
	(void)server;
	return _maxBodySize; }

// std::list<ErrorPages>	Server::get_errorPages() const { return _errorPages; }

/* ************************************************************************** */

std::list<Server> initServers(std::list<struct ServerSettings> settings, int epollFd)
{
	std::list<Server> servers;

	for (std::list<struct ServerSettings>::iterator it = settings.begin(); it != settings.end(); ++it)
	{
		servers.push_back(Server());
		if (servers.back().initServer(*it, epollFd) == 1)
			servers.pop_back();
	}
	return (servers);
}
