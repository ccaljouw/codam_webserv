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
#include <string.h>

// ============= con-/destructors ================

Server::Server() {}

Server::Server( Server const & src ) { (void)src; }

Server::~Server() {	close(_fd); }

// ============= Methods ================

int	Server::assign_name()
{
	if (bind(_fd, reinterpret_cast<struct sockaddr*>(&_serverAddr), sizeof(_serverAddr)) == -1) { // get adress
		std::cerr << " error in bind\n";
		return 1;
	}
	return 0;
}

int	Server::set_to_listen(int backlog)
{
	if (listen(_fd, backlog) == -1) {
		std::cerr << " error in listen\n";
		return 1;
	}
	return 0;
}

int Server::initServer(struct ServerSettings const & settings, int epollFd)
{
	_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	// _fd = 0;
	_port = settings._port;
	_serverName = settings._serverName;
	_locations = settings._locations;
    _serverAddr.sin_family = AF_INET;
	_serverAddr.sin_addr.s_addr = INADDR_ANY;
	_serverAddr.sin_port = htons(_port);

	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<struct sockaddr*>(&_serverAddr), sizeof(_serverAddr)))
	{
		std::cerr << "\033[31;1mError " << settings._serverName << "\n" << strerror(errno) << " in setsockopt\033[0m" << std::endl;
		return 1;
	}
	if (assign_name())
		return 1;
	if (set_to_listen(5))
		return 1;
	if (register_server(epollFd, this->_fd, this))
		return 1;
	std::cout << "\033[32;1mServer: " << settings._serverName << ", listening on port "  << _port << "\033[0m" << std::endl;
	return 0;
}

// ============= Getters ================
int	Server::get_FD() const { return _fd; }
std::string	Server::get_serverName() const { return _serverName; }
std::string	Server::get_rootFolder() const { return _rootFolder; }
std::string	Server::get_index() const { return _index; }
std::list<struct LocationSettings>	Server::get_locations() const { return _locations; }
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
