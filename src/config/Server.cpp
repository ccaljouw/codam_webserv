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

#include <webServ.hpp>
#include <Server.hpp>

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Server::Server()
{
}

Server::Server( const Server & src )
{
	(void)src;
}

Server::Server(uint16_t port, int epollFd)
{
	_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    _serverAddr.sin_family = AF_INET;
	_serverAddr.sin_addr.s_addr = INADDR_ANY;
	_serverAddr.sin_port = htons(port);
	_port = port; // remove?

	setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<struct sockaddr*>(&_serverAddr), sizeof(_serverAddr));
	assign_name();
	set_to_listen(5);
	register_server(epollFd, this->_fd);

	std::cout << "\033[32;1mServer listening on port " \
			  << port << "\033[0m" << std::endl;
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Server::~Server()
{
	close(_fd);
}


/*
** --------------------------------- OVERLOAD ---------------------------------
*/

Server &				Server::operator=( Server const & rhs )
{
	// if ( this != &rhs )
	// {
	// 	this->_value = rhs.getValue();
	// }
	(void)rhs;
	return *this;
}

std::ostream &			operator<<( std::ostream & o, Server const & i )
{
	//o << "Value = " << i.getValue();
	(void)i;
	return o;
}


/*
** --------------------------------- METHODS ----------------------------------
*/

void	Server::assign_name()
{
	bind(_fd, reinterpret_cast<struct sockaddr*>(&_serverAddr), sizeof(_serverAddr));
}

void	Server::set_to_listen(int backlog)
{
	listen(_fd, backlog);
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

int		Server::get_FD()
{
	return this->_fd;
}

/* ************************************************************************** */