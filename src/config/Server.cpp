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

Server::Server(struct ServerSettings const & settings, int epollFd)
{
	_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    _serverAddr.sin_family = AF_INET;
	_serverAddr.sin_addr.s_addr = INADDR_ANY;
	_port = settings._port; // remove?
	_serverAddr.sin_port = htons(_port);

	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<struct sockaddr*>(&_serverAddr), sizeof(_serverAddr)))
		std::cout << "error in setsockopt" << std::endl;
	assign_name();
	set_to_listen(5);
	if (register_server(epollFd, this->_fd, this))
		std::cout << "error in register event" << std::endl;;

	std::cout << "\033[32;1mServer: " << settings._serverName << ", listening on port "  << _port << "\033[0m" << std::endl;
}

Server::~Server()
{
	close(_fd);
}

/*
** --------------------------------- METHODS ----------------------------------
*/

void	Server::assign_name()
{
	if (bind(_fd, reinterpret_cast<struct sockaddr*>(&_serverAddr), sizeof(_serverAddr)))
		std::cout << "error in bind" << std::endl;
}

void	Server::set_to_listen(int backlog)
{
	if (listen(_fd, backlog))
		std::cout << "error in listen" << std::endl;;
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

int		Server::get_FD() const
{
	return this->_fd;
}

/* ************************************************************************** */

std::list<Server *> initServers(std::list<struct ServerSettings> settings, int epollFd)
{
	std::list<Server *> servers;

	for (std::list<struct ServerSettings>::iterator it = settings.begin(); it != settings.end(); ++it)
		servers.push_back(new Server(*it, epollFd));
	
	return (servers);
}